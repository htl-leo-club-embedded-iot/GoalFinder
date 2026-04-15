/*
 * ===============================================================================
 * (c) HTBLA Leonding 2024 - 2026
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Licensed under MIT License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the license.
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * All trademarks used in this document are property of their respective owners.
 * ===============================================================================
 */

import { defineStore } from "pinia";
import { ref, watch } from "vue";

export type EventCallback = () => void;

export const useWebSocketStore = defineStore("websocket", () => {
    let ws: WebSocket | null = null;
    let reconnectTimer: ReturnType<typeof setTimeout> | null = null;
    let connectTimeoutTimer: ReturnType<typeof setTimeout> | null = null;
    let reconnectAttempts = 0;
    let connectionEpoch = 0;
    let connectAttemptId = 0;
    let manualDisconnect = false;
    let candidateUrls: string[] = [];
    let candidateIndex = 0;

    const WS_DEFAULT_PORT = "81";
    const WS_CONNECT_TIMEOUT_MS = 2000;
    const WS_CONNECT_TIMEOUT_LOCAL_HOST_MS = 900;
    const WS_CONNECT_TIMEOUT_IP_MS = 1400;
    const WS_DNS_FALLBACK_MS = 550;
    const WS_LAST_URL_STORAGE_KEY = "goalfinder.ws.lastUrl";
    const GET_SETTINGS_STRIPPED_KEYS = new Set(["devicePassword", "extNWPWD", "externalNetworkPassword"]);
    const SETTINGS_KEY_ALIASES: Record<string, string[]> = {
        extNWUseDHCP: ["extNW_UseDHCP"],
        extNWIP: ["extNW_IP"],
        extNWSNM: ["extNW_SNM"],
        extNWDFG: ["extNW_DFG"],
        extNWDNSIP: ["extNW_DNSIP"],
    };

    const MAX_RECONNECT_DELAY = 5000;
    const BASE_RECONNECT_DELAY = 500;

    const isConnected = ref(false);
    const isAuthRequired = ref<boolean | null>(null);

    const eventListeners = new Map<string, Set<EventCallback>>();

    type MessageHandler = (msg: any) => void;
    type MessageMatcher = (msg: any) => boolean;
    type PendingHandler = {
        handler: MessageHandler;
        matcher?: MessageMatcher;
        reject?: (error: Error) => void;
        timeoutId?: ReturnType<typeof setTimeout> | null;
    };
    const pendingHandlers = new Map<string, PendingHandler[]>();

    function clearConnectTimeout(): void {
        if (connectTimeoutTimer) {
            clearTimeout(connectTimeoutTimer);
            connectTimeoutTimer = null;
        }
    }

    function clearPendingHandlers(reason: string): void {
        if (pendingHandlers.size === 0) return;

        pendingHandlers.forEach((handlers) => {
            handlers.forEach((entry) => {
                if (entry.timeoutId) {
                    clearTimeout(entry.timeoutId);
                    entry.timeoutId = null;
                }

                if (entry.reject) {
                    entry.reject(new Error(reason));
                    entry.reject = undefined;
                }
            });
        });

        pendingHandlers.clear();
    }

    function isIpHostname(hostname: string): boolean {
        const ipv4Pattern = /^\d{1,3}(\.\d{1,3}){3}$/;
        return ipv4Pattern.test(hostname) || hostname.includes(":");
    }

    function getHostnameFromUrl(url: string): string {
        try {
            return new URL(url).hostname.toLowerCase();
        } catch (error) {
            console.warn(error);
            return "";
        }
    }

    function getConnectTimeoutMs(wsUrl: string): number {
        const hostname = getHostnameFromUrl(wsUrl);
        if (hostname.endsWith(".local")) {
            return WS_CONNECT_TIMEOUT_LOCAL_HOST_MS;
        }
        if (isIpHostname(hostname)) {
            return WS_CONNECT_TIMEOUT_IP_MS;
        }

        return WS_CONNECT_TIMEOUT_MS;
    }

    function buildWsUrl(protocol: string, hostname: string, port: string): string | null {
        if (!hostname) return null;

        try {
            const url = new URL(window.location.href);
            url.protocol = protocol;
            url.hostname = hostname;
            url.port = port;
            url.pathname = "/";
            url.search = "";
            url.hash = "";
            return url.toString();
        } catch {
            return null;
        }
    }

    function getRememberedWsUrl(protocol: string): string | null {
        try {
            const remembered = localStorage.getItem(WS_LAST_URL_STORAGE_KEY);
            if (!remembered) return null;

            const url = new URL(remembered);
            url.protocol = protocol;
            if (!url.port) {
                url.port = WS_DEFAULT_PORT;
            }
            url.pathname = "/";
            url.search = "";
            url.hash = "";
            return url.toString();
        } catch {
            return null;
        }
    }

    function rememberWsUrl(url: string): void {
        try {
            localStorage.setItem(WS_LAST_URL_STORAGE_KEY, url);
        } catch {
            // Ignore storage access issues (private browsing / restricted contexts).
        }
    }

    function getWsCandidateUrls(): string[] {
        const protocol = window.location.protocol === "https:" ? "wss:" : "ws:";
        const urls: string[] = [];
        const currentLocation = new URL(window.location.href);
        const currentHost = currentLocation.hostname.toLowerCase();
        const isLocalAliasHost = currentHost === "goalfinder.local" || currentHost.endsWith(".goalfinder.local");

        const addCandidate = (candidate: string | null) => {
            if (!candidate || urls.includes(candidate)) return;
            urls.push(candidate);
        };

        addCandidate(buildWsUrl(protocol, currentLocation.hostname, WS_DEFAULT_PORT));

        if (currentLocation.port && currentLocation.port !== "80" && currentLocation.port !== "443") {
            addCandidate(buildWsUrl(protocol, currentLocation.hostname, currentLocation.port));
        }

        if (isLocalAliasHost) {
            addCandidate(buildWsUrl(protocol, "192.168.4.1", WS_DEFAULT_PORT));
        }

        addCandidate(getRememberedWsUrl(protocol));
        addCandidate(buildWsUrl(protocol, "goalfinder.local", WS_DEFAULT_PORT));
        addCandidate(buildWsUrl(protocol, "192.168.4.1", WS_DEFAULT_PORT));

        return urls;
    }

    function tryConnectCandidate(epoch: number): void {
        if (epoch !== connectionEpoch || manualDisconnect) return;

        if (candidateIndex >= candidateUrls.length) {
            ws = null;
            scheduleReconnect();
            return;
        }

        const wsUrl = candidateUrls[candidateIndex];
        const timeoutMs = getConnectTimeoutMs(wsUrl);
        const attemptId = ++connectAttemptId;
        const currentHostname = getHostnameFromUrl(wsUrl);
        const nextCandidate = candidateIndex + 1 < candidateUrls.length ? candidateUrls[candidateIndex + 1] : null;
        const nextHostname = nextCandidate ? getHostnameFromUrl(nextCandidate) : "";
        const shouldApplyDnsFallback = currentHostname.endsWith(".local") && isIpHostname(nextHostname);
        let opened = false;
        let dnsFallbackTimer: ReturnType<typeof setTimeout> | null = null;
        let socket: WebSocket;

        console.log(`[WS] Connecting (${candidateIndex + 1}/${candidateUrls.length}): ${wsUrl}`);

        try {
            socket = new WebSocket(wsUrl);
            ws = socket;
        } catch (error) {
            console.error(`[WS] Failed to create socket for ${wsUrl}:`, error);
            candidateIndex++;
            tryConnectCandidate(epoch);
            return;
        }

        const isCurrentAttempt = () =>
            epoch === connectionEpoch && !manualDisconnect && attemptId === connectAttemptId;

        const clearDnsFallbackTimer = () => {
            if (dnsFallbackTimer) {
                clearTimeout(dnsFallbackTimer);
                dnsFallbackTimer = null;
            }
        };

        const advanceToNextCandidate = (reason: string) => {
            if (isCurrentAttempt()) {
                console.warn(`[WS] ${reason}: ${wsUrl}`);
                connectAttemptId++;
                clearConnectTimeout();
                clearDnsFallbackTimer();

                socket.onopen = null;
                socket.onclose = null;
                socket.onerror = null;
                socket.onmessage = null;

                try {
                    socket.close();
                } catch (e) {
                    console.warn(e);
                }

                candidateIndex++;
                tryConnectCandidate(epoch);
            }
        };

        clearConnectTimeout();
        connectTimeoutTimer = setTimeout(() => {
            if (isCurrentAttempt() && socket.readyState === WebSocket.CONNECTING) {   
                advanceToNextCandidate(`Connection timeout (${timeoutMs}ms)`);
            }
        }, timeoutMs);

        if (shouldApplyDnsFallback) {
            dnsFallbackTimer = setTimeout(() => {
                if (isCurrentAttempt() && socket.readyState === WebSocket.CONNECTING) {
                    advanceToNextCandidate(`DNS fallback (${WS_DNS_FALLBACK_MS}ms)`);
                }
            }, WS_DNS_FALLBACK_MS);
        }

        socket.onopen = () => {
            if (!isCurrentAttempt()) {
                socket.close();
                return;
            }

            opened = true;
            clearConnectTimeout();
            clearDnsFallbackTimer();
            isConnected.value = true;
            reconnectAttempts = 0;
            rememberWsUrl(wsUrl);
            ws = socket;

            console.log(`[WS] Connected: ${wsUrl}`);
            send({ type: "is_auth" });
        };

        socket.onclose = () => {
            if (epoch !== connectionEpoch || attemptId !== connectAttemptId) return;

            clearConnectTimeout();
            clearDnsFallbackTimer();
            const wasConnected = opened || isConnected.value;
            isConnected.value = false;
            if (ws === socket) {
                ws = null;
            }

            console.log(`[WS] Disconnected: ${wsUrl}`);

            if (manualDisconnect) {
                clearPendingHandlers("WebSocket disconnected");
                return;
            }

            clearPendingHandlers("WebSocket connection lost");

            if (!wasConnected && candidateIndex + 1 < candidateUrls.length) {
                candidateIndex++;
                tryConnectCandidate(epoch);
                return;
            }

            scheduleReconnect();
        };

        socket.onerror = (error) => {
            if (!isCurrentAttempt()) return;
            console.error(`[WS] Error on ${wsUrl}:`, error);
        };

        socket.onmessage = (event: MessageEvent) => {
            if (!isCurrentAttempt()) return;
            handleMessage(event.data);
        };
    }

    function connect(): void {
        manualDisconnect = false;

        if (ws && (ws.readyState === WebSocket.OPEN || ws.readyState === WebSocket.CONNECTING)) {
            return;
        }

        candidateUrls = getWsCandidateUrls();
        if (candidateUrls.length === 0) {
            console.error("[WS] No valid WebSocket endpoints available");
            scheduleReconnect();
            return;
        }

        candidateIndex = 0;
        connectionEpoch++;
        tryConnectCandidate(connectionEpoch);
    }

    function disconnect(): void {
        manualDisconnect = true;
        connectionEpoch++;
        connectAttemptId++;
        candidateUrls = [];
        candidateIndex = 0;

        if (reconnectTimer) {
            clearTimeout(reconnectTimer);
            reconnectTimer = null;
        }

        clearConnectTimeout();
        clearPendingHandlers("WebSocket disconnected");

        if (ws) {
            const socket = ws;
            ws = null;

            socket.onopen = null;
            socket.onclose = null;
            socket.onerror = null;
            socket.onmessage = null;

            try {
                socket.close(1000, "manual disconnect");
            } catch {
                socket.close();
            }
        }

        isConnected.value = false;
    }

    function waitForConnection(timeoutMs: number): Promise<void> {
        return new Promise((resolve, reject) => {
            if (ws?.readyState === WebSocket.OPEN || isConnected.value) {
                resolve();
            } else {
                if (!ws || ws.readyState === WebSocket.CLOSED || ws.readyState === WebSocket.CLOSING) {
                    connect();
                }

                let finished = false;
                let timer: ReturnType<typeof setTimeout> | null = null;

                const stopWatch = watch(isConnected, (connected) => {
                    if (!connected || finished) return;
                    finished = true;
                    if (timer) {
                        clearTimeout(timer);
                    }
                    stopWatch();
                    resolve();
                });

                timer = setTimeout(() => {
                    if (finished) return;
                    finished = true;
                    stopWatch();
                    reject(new Error("Timeout waiting for WebSocket connection"));
                }, timeoutMs);
            }
        });
    }

    function scheduleReconnect(): void {
        if (manualDisconnect) return;
        if (reconnectTimer) return;

        const delay = Math.min(BASE_RECONNECT_DELAY * Math.pow(2, reconnectAttempts), MAX_RECONNECT_DELAY);
        reconnectAttempts++;

        reconnectTimer = setTimeout(() => {
            reconnectTimer = null;
            connect();
        }, delay);
    }

    function sanitizeSettingsData(data: Record<string, any>): Record<string, any> {
        const sanitized: Record<string, any> = { ...data };
        GET_SETTINGS_STRIPPED_KEYS.forEach((key) => {
            if (Object.prototype.hasOwnProperty.call(sanitized, key)) {
                delete sanitized[key];
            }
        });

        Object.entries(SETTINGS_KEY_ALIASES).forEach(([canonicalKey, aliases]) => {
            if (Object.prototype.hasOwnProperty.call(sanitized, canonicalKey)) return;

            for (const alias of aliases) {
                if (Object.prototype.hasOwnProperty.call(sanitized, alias)) {
                    sanitized[canonicalKey] = sanitized[alias];
                    break;
                }
            }
        });

        if (Object.prototype.hasOwnProperty.call(sanitized, "extNWUseDHCP")) {
            const rawValue = sanitized["extNWUseDHCP"];
            if (typeof rawValue === "number") {
                sanitized["extNWUseDHCP"] = rawValue !== 0;
            } else if (typeof rawValue === "string") {
                sanitized["extNWUseDHCP"] = rawValue === "1" || rawValue.toLowerCase() === "true";
            }
        }

        return sanitized;
    }

    function handleMessage(raw: string): void {
        let msg: any;
        try {
            msg = JSON.parse(raw);
        } catch {
            console.warn("[WS] Invalid JSON:", raw);
            return;
        }

        const type = msg.type as string;
        if (!type) return;

        if (type === "event" && msg.event) {
            emit(msg.event);
        }

        if (type === "is_auth_result") {
            isAuthRequired.value = msg.isPasswordProtected ?? false;
        }

        if (type === "connected") {
            emit("connected");
        }

        // Dispatch to pending response handlers
        const handlers = pendingHandlers.get(type);
        if (handlers && handlers.length > 0) {
            const matchedIndex = handlers.findIndex((entry) => !entry.matcher || entry.matcher(msg));
            if (matchedIndex >= 0) {
                const [entry] = handlers.splice(matchedIndex, 1);
                if (handlers.length === 0) pendingHandlers.delete(type);
                entry.handler(msg);
            }
        }

        emit(type, msg);
    }

    function on(event: string, callback: EventCallback): void {
        if (!eventListeners.has(event)) {
            eventListeners.set(event, new Set());
        }
        eventListeners.get(event)!.add(callback);
    }

    function off(event: string, callback: EventCallback): void {
        eventListeners.get(event)?.delete(callback);
    }

    function emit(event: string, _data?: any): void {
        eventListeners.get(event)?.forEach((cb) => cb());
    }


    function send(data: object): void {
        if (ws?.readyState === WebSocket.OPEN) {
            ws.send(JSON.stringify(data));
        }
    }

    async function sendAndWait(data: object, responseType: string, timeoutMs = 5000, matcher?: MessageMatcher): Promise<any> {
        await waitForConnection(timeoutMs);

        return new Promise((resolve, reject) => {
            const pending: PendingHandler = {
                matcher,
                handler: (msg: any) => {
                    if (pending.timeoutId) {
                        clearTimeout(pending.timeoutId);
                        pending.timeoutId = null;
                    }
                    pending.reject = undefined;
                    resolve(msg);
                },
                reject,
            };

            const timer = setTimeout(() => {
                const handlers = pendingHandlers.get(responseType);
                if (handlers) {
                    const idx = handlers.indexOf(pending);
                    if (idx >= 0) handlers.splice(idx, 1);
                    if (handlers.length === 0) pendingHandlers.delete(responseType);
                }
                pending.reject = undefined;
                reject(new Error(`Timeout waiting for ${responseType}`));
            }, timeoutMs);

            pending.timeoutId = timer;

            if (!pendingHandlers.has(responseType)) {
                pendingHandlers.set(responseType, []);
            }
            pendingHandlers.get(responseType)!.push(pending);

            send(data);
        });
    }

    function loadSettings(timeoutMs = 5000): Promise<Record<string, any>> {
        return sendAndWait({ type: "get_settings" }, "settings", timeoutMs)
            .then((msg: any) => sanitizeSettingsData(msg?.data ?? {}));
    }

    function sendStart(): void {
        send({ type: "start" });
    }

    function sendStop(): void {
        send({ type: "stop" });
    }

    function sendGetSettings(): void {
        send({ type: "get_settings" });
    }

    function sendSetSetting(key: string, value: any): void {
        send({ type: "set", key, value });
    }

    function setSettingAndWait(key: string, value: any, timeoutMs = 5000): Promise<any> {
        return sendAndWait(
            { type: "set", key, value },
            "setting_ack",
            timeoutMs,
            (msg: any) => msg?.key === key,
        );
    }

    function sendRestart(): void {
        send({ type: "restart" });
    }

    function sendFactoryReset(): void {
        send({ type: "factory_reset" });
    }

    async function sendAuth(passwordHash: string): Promise<any> {
        return sendAndWait({ type: "auth", passwordHash }, "auth_result");
    }

    function sendPing(): void {
        send({ type: "ping" });
    }

    function waitForAuthStatus(): Promise<boolean> {
        return new Promise((resolve) => {
            if (isAuthRequired.value !== null) {
                resolve(isAuthRequired.value);
                return;
            }

            if (!ws || ws.readyState === WebSocket.CLOSED || ws.readyState === WebSocket.CLOSING) {
                connect();
            }

            const unwatch = watch(isAuthRequired, (val) => {
                if (val !== null) {
                    unwatch();
                    resolve(val);
                }
            });

            setTimeout(() => {
                if (isAuthRequired.value === null) {
                    unwatch();
                    isAuthRequired.value = false;
                    resolve(false);
                }
            }, 5000);
        });
    }

    return {
        isConnected,
        isAuthRequired,
        connect,
        disconnect,
        on,
        off,
        send,
        sendAndWait,
        loadSettings,
        sendStart,
        sendStop,
        sendGetSettings,
        sendSetSetting,
        setSettingAndWait,
        sendRestart,
        sendFactoryReset,
        sendAuth,
        sendPing,
        waitForAuthStatus,
    };
});

export type WebSocketStore = ReturnType<typeof useWebSocketStore>;
