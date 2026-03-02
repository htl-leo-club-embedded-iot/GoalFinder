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
    let reconnectAttempts = 0;
    const MAX_RECONNECT_DELAY = 5000;
    const BASE_RECONNECT_DELAY = 500;

    const isConnected = ref(false);
    const isAuthRequired = ref<boolean | null>(null);

    const eventListeners = new Map<string, Set<EventCallback>>();

    type MessageHandler = (msg: any) => void;
    const pendingHandlers = new Map<string, MessageHandler[]>();

    function getWsUrl(): string {
        const protocol = window.location.protocol === "https:" ? "wss:" : "ws:";
        return `${protocol}//${window.location.hostname}:81/`;
    }

    function connect(): void {
        if (ws && (ws.readyState === WebSocket.OPEN || ws.readyState === WebSocket.CONNECTING)) {
            return;
        }

        try {
            ws = new WebSocket(getWsUrl());

            ws.onopen = () => {
                isConnected.value = true;
                reconnectAttempts = 0;
                console.log("[WS] Connected");
                send({ type: "is_auth" });
            };

            ws.onclose = () => {
                isConnected.value = false;
                console.log("[WS] Disconnected");
                scheduleReconnect();
            };

            ws.onerror = (error) => {
                console.error("[WS] Error:", error);
            };

            ws.onmessage = (event: MessageEvent) => {
                handleMessage(event.data);
            };
        } catch (error) {
            console.error("[WS] Connection failed:", error);
            scheduleReconnect();
        }
    }

    function disconnect(): void {
        if (reconnectTimer) {
            clearTimeout(reconnectTimer);
            reconnectTimer = null;
        }
        if (ws) {
            ws.onclose = null;
            ws.close();
            ws = null;
        }
        isConnected.value = false;
    }

    function scheduleReconnect(): void {
        if (reconnectTimer) return;
        const delay = Math.min(BASE_RECONNECT_DELAY * Math.pow(2, reconnectAttempts), MAX_RECONNECT_DELAY);
        reconnectAttempts++;
        reconnectTimer = setTimeout(() => {
            reconnectTimer = null;
            connect();
        }, delay);
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
            const handler = handlers.shift()!;
            if (handlers.length === 0) pendingHandlers.delete(type);
            handler(msg);
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

    function sendAndWait(data: object, responseType: string, timeoutMs = 5000): Promise<any> {
        return new Promise((resolve, reject) => {
            const timer = setTimeout(() => {
                const handlers = pendingHandlers.get(responseType);
                if (handlers) {
                    const idx = handlers.indexOf(handler);
                    if (idx >= 0) handlers.splice(idx, 1);
                }
                reject(new Error(`Timeout waiting for ${responseType}`));
            }, timeoutMs);

            const handler = (msg: any) => {
                clearTimeout(timer);
                resolve(msg);
            };

            if (!pendingHandlers.has(responseType)) {
                pendingHandlers.set(responseType, []);
            }
            pendingHandlers.get(responseType)!.push(handler);

            send(data);
        });
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

    function sendRestart(): void {
        send({ type: "restart" });
    }

    function sendFactoryReset(): void {
        send({ type: "factory_reset" });
    }

    async function sendAuth(password: string): Promise<any> {
        return sendAndWait({ type: "auth", password }, "auth_result");
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

            if (!ws || ws.readyState === WebSocket.CLOSED) {
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
        sendStart,
        sendStop,
        sendGetSettings,
        sendSetSetting,
        sendRestart,
        sendFactoryReset,
        sendAuth,
        sendPing,
        waitForAuthStatus,
    };
});

export type WebSocketStore = ReturnType<typeof useWebSocketStore>;
