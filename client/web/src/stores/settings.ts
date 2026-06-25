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

import {defineStore} from "pinia";
import {ref} from "vue";
import {useWebSocketStore} from "@/stores/websocket";

const API_URL = "/api";
const SECRET_SETTING_KEYS = new Set(["devicePassword", "wifiPassword", "extNWPWD", "extNWEnterprisePassword"]);
const SECRET_PRESENT_PLACEHOLDER = "***";

export const useSettingsStore = defineStore("settings", () => {
    let isLoading = false;
    let saveTimeout: ReturnType<typeof setTimeout> | null = null;
    const enableDarkMode = ref(false);
    const isSoundEnabled = ref(false);
    let serverSnapshot: Record<string, any> = {};

    //General
    const deviceName = ref("");
    const devicePassword = ref("");
    const wifiPassword = ref("");
    
    // LED
    const ledMode = ref(0);
    const ledModeStr = ref("Aus");
    const ledBrightness = ref(100);

    //Devices
    const isBluetoothEnabled = ref(false);
    const connectedBluetoothDevices = ref([]);
    const availableBluetoothDevices = ref([]);
    
    const vibrationSensorSensitivity = ref(0);
    const ballHitDetectionDistance = ref(180);
    const distanceOnlyHitDetection = ref(false);
    const afterHitTimeout = ref(5);

    const isWifiEnabled = ref(false);
    const connectedNetwork = ref("");
    const availableNetworks = ref([]);

    //Audio
    const volume = ref(0);
    const metronomeSound = ref(0);
    const hitSound = ref(0);
    const missSound = ref(0);
    const waitingSound = ref(0);
    const metronomeSoundDelay = ref(1500);

    //System
    const macAddress = ref("");
    const version = ref("");
    const advancedSettingsEnabled = ref(false);
    const dnsEnabled = ref(true);
    const externalNetworkEnabled = ref(false);
    const externalNetworkSsid = ref("");
    const externalNetworkPassword = ref("");
    const externalNetworkUseDhcp = ref(true);
    const externalNetworkIp = ref("");
    const externalNetworkSubnetMask = ref("");
    const externalNetworkDefaultGateway = ref("");
    const externalNetworkDnsIp = ref("");
    const externalNetworkAuthMode = ref("wpa2-personal");
    const externalNetworkEnterpriseIdentity = ref("");
    const externalNetworkEnterpriseUsername = ref("");
    const externalNetworkEnterpriseAnonymousIdentity = ref("");
    const externalNetworkEnterprisePassword = ref("");
    const externalNetworkEnterprisePhase2Method = ref("auto");
    const externalNetworkEnterpriseCaCertificate = ref("");
    const externalNetworkEnterpriseClientCertificate = ref("");
    const externalNetworkEnterpriseClientPrivateKey = ref("");

    const refreshAvailableNetworks = () => {};
    const refreshAvailableBluetoothDevices = () => {};

    /** Maps setting keys to their reactive refs */
    function getSettingsMap(): Record<string, any> {
        return {
            deviceName: deviceName.value,
            devicePassword: devicePassword.value,
            wifiPassword: wifiPassword.value,
            volume: volume.value,
            metronomeSound: metronomeSound.value,
            hitSound: hitSound.value,
            missSound: missSound.value,
            waitingSound: waitingSound.value,
            metSoundDelay: metronomeSoundDelay.value,
            ledMode: ledMode.value,
            ledBrightness: ledBrightness.value,
            vibrationSensorSensitivity: 100 - vibrationSensorSensitivity.value,
            ballHitDetectionDistance: ballHitDetectionDistance.value,
            distanceOnlyHitDetection: distanceOnlyHitDetection.value,
            afterHitTimeout: afterHitTimeout.value,
            isSoundEnabled: isSoundEnabled.value,
            advancedSettingsEnabled: advancedSettingsEnabled.value,
            DNSEnabled: dnsEnabled.value,
            extNW: externalNetworkEnabled.value,
            extNWSSID: externalNetworkSsid.value,
            extNWPWD: externalNetworkPassword.value,
            extNWUseDHCP: externalNetworkUseDhcp.value,
            extNWIP: externalNetworkIp.value,
            extNWSNM: externalNetworkSubnetMask.value,
            extNWDFG: externalNetworkDefaultGateway.value,
            extNWDNSIP: externalNetworkDnsIp.value,
            extNWAuthMode: externalNetworkAuthMode.value,
            extNWEnterpriseIdentity: externalNetworkEnterpriseIdentity.value,
            extNWEnterpriseUsername: externalNetworkEnterpriseUsername.value,
            extNWEnterpriseAnonymousIdentity: externalNetworkEnterpriseAnonymousIdentity.value,
            extNWEnterprisePassword: externalNetworkEnterprisePassword.value,
            extNWEnterprisePhase2Method: externalNetworkEnterprisePhase2Method.value,
            extNWEnterpriseCaCertificate: externalNetworkEnterpriseCaCertificate.value,
            extNWEnterpriseClientCertificate: externalNetworkEnterpriseClientCertificate.value,
            extNWEnterpriseClientPrivateKey: externalNetworkEnterpriseClientPrivateKey.value,
        };
    }

    /** Apply settings data from server response */
    function applySettingsData(json: Record<string, any>): void {
        const hasWifiPassword = json["wifiPasswordSet"] === true;
        const hasDevicePassword = json["devicePasswordSet"] === true;
        const hasExternalNetworkPassword = json["extNWPasswordSet"] === true;
        const hasEnterprisePassword = json["extNWEnterprisePasswordSet"] === true;

        deviceName.value = json["deviceName"] ?? deviceName.value;
        if (Object.prototype.hasOwnProperty.call(json, "devicePassword")) {
            devicePassword.value = json["devicePassword"] ?? devicePassword.value;
        } else {
            devicePassword.value = hasDevicePassword ? SECRET_PRESENT_PLACEHOLDER : "";
        }
        if (Object.prototype.hasOwnProperty.call(json, "wifiPassword")) {
            wifiPassword.value = json["wifiPassword"] ?? wifiPassword.value;
        } else {
            wifiPassword.value = hasWifiPassword ? SECRET_PRESENT_PLACEHOLDER : "";
        }
        volume.value = json["volume"] ?? volume.value;
        metronomeSound.value = json["metronomeSound"] ?? metronomeSound.value;
        hitSound.value = json["hitSound"] ?? hitSound.value;
        missSound.value = json["missSound"] ?? missSound.value;
        waitingSound.value = json["waitingSound"] ?? waitingSound.value;
        metronomeSoundDelay.value = json["metSoundDelay"] ?? metronomeSoundDelay.value;
        ledMode.value = json["ledMode"] ?? ledMode.value;
        ledBrightness.value = json["ledBrightness"] ?? ledBrightness.value;
        macAddress.value = json["macAddress"] ?? macAddress.value;
        isSoundEnabled.value = json["isSoundEnabled"] ?? isSoundEnabled.value;
        version.value = json["version"] ?? version.value;
        vibrationSensorSensitivity.value = json["vibrationSensorSensitivity"] !== undefined
          ? 100 - json["vibrationSensorSensitivity"]
          : vibrationSensorSensitivity.value;
        ballHitDetectionDistance.value = json["ballHitDetectionDistance"] ?? ballHitDetectionDistance.value;
        distanceOnlyHitDetection.value = json["distanceOnlyHitDetection"] ?? false;
        afterHitTimeout.value = json["afterHitTimeout"] ?? 5;
        advancedSettingsEnabled.value = json["advancedSettingsEnabled"] ?? false;
        dnsEnabled.value = json["DNSEnabled"] ?? dnsEnabled.value;
        externalNetworkEnabled.value = json["extNW"] ?? json["useExternalNW"] ?? externalNetworkEnabled.value;
        externalNetworkSsid.value = json["extNWSSID"] ?? externalNetworkSsid.value;
        if (Object.prototype.hasOwnProperty.call(json, "extNWPWD")) {
            externalNetworkPassword.value = json["extNWPWD"] ?? externalNetworkPassword.value;
        } else {
            externalNetworkPassword.value = hasExternalNetworkPassword ? SECRET_PRESENT_PLACEHOLDER : "";
        }
        externalNetworkUseDhcp.value = json["extNWUseDHCP"] ?? externalNetworkUseDhcp.value;
        externalNetworkIp.value = json["extNWIP"] ?? externalNetworkIp.value;
        externalNetworkSubnetMask.value = json["extNWSNM"] ?? externalNetworkSubnetMask.value;
        externalNetworkDefaultGateway.value = json["extNWDFG"] ?? externalNetworkDefaultGateway.value;
        externalNetworkDnsIp.value = json["extNWDNSIP"] ?? externalNetworkDnsIp.value;
        externalNetworkAuthMode.value = json["extNWAuthMode"] ?? externalNetworkAuthMode.value;
        externalNetworkEnterpriseIdentity.value = json["extNWEnterpriseIdentity"] ?? externalNetworkEnterpriseIdentity.value;
        externalNetworkEnterpriseUsername.value = json["extNWEnterpriseUsername"] ?? externalNetworkEnterpriseUsername.value;
        externalNetworkEnterpriseAnonymousIdentity.value = json["extNWEnterpriseAnonymousIdentity"] ?? externalNetworkEnterpriseAnonymousIdentity.value;
        if (Object.prototype.hasOwnProperty.call(json, "extNWEnterprisePassword")) {
            externalNetworkEnterprisePassword.value = json["extNWEnterprisePassword"] ?? externalNetworkEnterprisePassword.value;
        } else {
            externalNetworkEnterprisePassword.value = hasEnterprisePassword ? SECRET_PRESENT_PLACEHOLDER : "";
        }
        externalNetworkEnterprisePhase2Method.value = json["extNWEnterprisePhase2Method"] ?? externalNetworkEnterprisePhase2Method.value;
        externalNetworkEnterpriseCaCertificate.value = json["extNWEnterpriseCaCertificate"] ?? externalNetworkEnterpriseCaCertificate.value;
        externalNetworkEnterpriseClientCertificate.value = json["extNWEnterpriseClientCertificate"] ?? externalNetworkEnterpriseClientCertificate.value;
        externalNetworkEnterpriseClientPrivateKey.value = json["extNWEnterpriseClientPrivateKey"] ?? externalNetworkEnterpriseClientPrivateKey.value;

        // Update LED mode string
        const ledModeMapping: { [key: number]: string } = {
            1: "Ein", 2: "Fade", 3: "Blitzartig", 4: "Turbo", 5: "Aus"
        };
        ledModeStr.value = ledModeMapping[ledMode.value] || "Unknown";

        // Take a snapshot of the server state
        serverSnapshot = getSettingsMap();
    }

    /**
     * Request all settings from the device via WebSocket.
     * Listens for the "settings" message type.
     */
    function getSettings(): void {
        isLoading = true;
        const wsStore = useWebSocketStore();

        wsStore.loadSettings(5000)
            .then((data: Record<string, any>) => {
                applySettingsData(data);
            })
            .catch((err: any) => {
                console.error("[Settings] Failed to get settings:", err);
            })
            .finally(() => {
                isLoading = false;
            });
    }

    async function syncChangedSettings(): Promise<void> {
        if (isLoading) return;

        const wsStore = useWebSocketStore();
        const currentSettings = getSettingsMap();

        for (const key of Object.keys(currentSettings)) {
            if (currentSettings[key] !== serverSnapshot[key]) {
                try {
                    const ack = await wsStore.setSettingAndWait(key, currentSettings[key], 5000);
                    if (SECRET_SETTING_KEYS.has(key)) {
                        serverSnapshot[key] = currentSettings[key];
                    } else {
                        serverSnapshot[key] = ack?.value ?? currentSettings[key];
                    }
                } catch (err: any) {
                    console.error(`[Settings] Failed to set '${key}':`, err);
                }
            }
        }
    }

    async function restartDevice(): Promise<void> {
        const wsStore = useWebSocketStore();
        if (saveTimeout) clearTimeout(saveTimeout);
        await syncChangedSettings();
        wsStore.sendRestart();
    }

    function factoryResetDevice(): void {
        const wsStore = useWebSocketStore();
        wsStore.sendFactoryReset();
    }

    /**
     * Firmware update via HTTP (stays as HTTP POST since it needs
     * streaming file upload which is better suited for HTTP).
     */
    function updateFirmware(
        firmwareFile: File,
        onProgress?: (percent: number) => void,
        onSuccess?: () => void,
        onError?: () => void
    ): void {
        const data = new FormData();
        data.append('file', firmwareFile);

        let uploadComplete = false;

        const xhr = new XMLHttpRequest();
        xhr.open('POST', `${API_URL}/update`);

        xhr.upload.addEventListener('progress', (e) => {
            if (e.lengthComputable && onProgress) {
                const percent = Math.round((e.loaded / e.total) * 100);
                onProgress(percent);
                if (percent >= 100) {
                    uploadComplete = true;
                }
            }
        });

        function pollUpdateStatus() {
            let attempts = 0;
            const maxAttempts = 30;
            const interval = 2000;

            const poll = () => {
                attempts++;
                fetch(`${API_URL}/update-status`, { signal: AbortSignal.timeout(3000) })
                    .then(res => res.json())
                    .then(data => {
                        if (data.updateSuccess) {
                            onSuccess?.();
                        } else if (attempts < maxAttempts) {
                            setTimeout(poll, interval);
                        } else {
                            onError?.();
                        }
                    })
                    .catch(() => {
                        if (attempts < maxAttempts) {
                            setTimeout(poll, interval);
                        } else {
                            onError?.();
                        }
                    });
            };

            setTimeout(poll, 5000);
        }

        xhr.addEventListener('load', () => {
            pollUpdateStatus();
        });

        xhr.addEventListener('error', () => {
            if (uploadComplete) pollUpdateStatus();
            else onError?.();
        });

        xhr.addEventListener('abort', () => {
            if (uploadComplete) pollUpdateStatus();
            else onError?.();
        });

        xhr.send(data);
    }
    
    function scheduleSave(): void {
        if (isLoading) return;
        if (saveTimeout) clearTimeout(saveTimeout);
        saveTimeout = setTimeout(() => {
            void syncChangedSettings();
        }, 500);
    }

    return {
        enableDarkMode,
        deviceName,
        devicePassword,
        wifiPassword,
        isBluetoothEnabled,
        connectedBluetoothDevices,
        availableBluetoothDevices,
        vibrationSensorSensitivity,
        ballHitDetectionDistance,
        distanceOnlyHitDetection,
        afterHitTimeout,
        isWifiEnabled,
        connectedNetwork,
        availableNetworks,
        volume,
        metronomeSound,
        metronomeSoundDelay,
        hitSound,
        missSound,
        waitingSound,
        macAddress,
        refreshAvailableNetworks,
        refreshAvailableBluetoothDevices,
        getSettings,
        syncChangedSettings,
        scheduleSave,
        restartDevice,
        factoryResetDevice,
        ledMode,
        ledModeStr,
        ledBrightness,
        isSoundEnabled,
        version,
        updateFirmware,
        advancedSettingsEnabled,
        dnsEnabled,
        externalNetworkEnabled,
        externalNetworkSsid,
        externalNetworkPassword,
        externalNetworkUseDhcp,
        externalNetworkIp,
        externalNetworkSubnetMask,
        externalNetworkDefaultGateway,
        externalNetworkDnsIp,
        externalNetworkAuthMode,
        externalNetworkEnterpriseIdentity,
        externalNetworkEnterpriseUsername,
        externalNetworkEnterpriseAnonymousIdentity,
        externalNetworkEnterprisePassword,
        externalNetworkEnterprisePhase2Method,
        externalNetworkEnterpriseCaCertificate,
        externalNetworkEnterpriseClientCertificate,
        externalNetworkEnterpriseClientPrivateKey,
    };
});