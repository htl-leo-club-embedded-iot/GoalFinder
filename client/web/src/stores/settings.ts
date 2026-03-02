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

    //System
    const macAddress = ref("");
    const version = ref("");

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
            ledMode: ledMode.value,
            ledBrightness: ledBrightness.value,
            vibrationSensorSensitivity: vibrationSensorSensitivity.value,
            ballHitDetectionDistance: ballHitDetectionDistance.value,
            distanceOnlyHitDetection: distanceOnlyHitDetection.value,
            afterHitTimeout: afterHitTimeout.value,
            isSoundEnabled: isSoundEnabled.value,
        };
    }

    /** Apply settings data from server response */
    function applySettingsData(json: Record<string, any>): void {
        deviceName.value = json["deviceName"] ?? deviceName.value;
        devicePassword.value = json["devicePassword"] ?? devicePassword.value;
        wifiPassword.value = json["wifiPassword"] ?? wifiPassword.value;
        volume.value = json["volume"] ?? volume.value;
        metronomeSound.value = json["metronomeSound"] ?? metronomeSound.value;
        hitSound.value = json["hitSound"] ?? hitSound.value;
        missSound.value = json["missSound"] ?? missSound.value;
        ledMode.value = json["ledMode"] ?? ledMode.value;
        ledBrightness.value = json["ledBrightness"] ?? ledBrightness.value;
        macAddress.value = json["macAddress"] ?? macAddress.value;
        isSoundEnabled.value = json["isSoundEnabled"] ?? isSoundEnabled.value;
        version.value = json["version"] ?? version.value;
        vibrationSensorSensitivity.value = json["vibrationSensorSensitivity"] ?? vibrationSensorSensitivity.value;
        ballHitDetectionDistance.value = json["ballHitDetectionDistance"] ?? ballHitDetectionDistance.value;
        distanceOnlyHitDetection.value = json["distanceOnlyHitDetection"] ?? false;
        afterHitTimeout.value = json["afterHitTimeout"] ?? 5;

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

        // Register a one-time handler for the settings response
        const handler = () => {
            // Handler registered via on/off pattern
        };

        wsStore.sendAndWait({ type: "get_settings" }, "settings", 5000)
            .then((msg: any) => {
                if (msg.data) {
                    applySettingsData(msg.data);
                }
            })
            .catch((err: any) => {
                console.error("[Settings] Failed to get settings:", err);
            })
            .finally(() => {
                isLoading = false;
            });
    }

    function syncChangedSettings(): void {
        if (isLoading) return;

        const wsStore = useWebSocketStore();
        const currentSettings = getSettingsMap();

        for (const key of Object.keys(currentSettings)) {
            if (currentSettings[key] !== serverSnapshot[key]) {
                wsStore.sendSetSetting(key, currentSettings[key]);
                serverSnapshot[key] = currentSettings[key];
            }
        }
    }

    function restartDevice(): void {
        const wsStore = useWebSocketStore();
        if (saveTimeout) clearTimeout(saveTimeout);
        syncChangedSettings();
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
            syncChangedSettings();
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
        hitSound,
        missSound,
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
        updateFirmware
    };
});