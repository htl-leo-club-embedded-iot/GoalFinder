import {defineStore} from "pinia";
import {ref} from "vue";

const API_URL = "/api"
const WIFI_PASSWORD_MIN_LENGTH = 8;
const WIFI_PASSWORD_MAX_LENGTH = 63;

function isWifiPasswordValid(password: string): boolean {
    return password.length === 0 || (password.length >= WIFI_PASSWORD_MIN_LENGTH && password.length <= WIFI_PASSWORD_MAX_LENGTH);
}

export const useSettingsStore = defineStore("settings", () => {
    let isValid = false;
    const enableDarkMode = ref(false);
    const isSoundEnabled = ref(false);

    //General
    const deviceName = ref("");
    const devicePassword = ref("");
    const wifiPassword = ref("");
    const lastValidWifiPassword = ref("");
    
    // LED
    const ledMode = ref(0);
    const ledModeStr = ref("Aus");
    const ledBrightness = ref(100);

    //Devices
    const isBluetoothEnabled = ref(false);
    const connectedBluetoothDevices = ref([]);
    const availableBluetoothDevices = ref([]);
    
    const vibrationSensorSensitivity = ref(0);
    const ballHitDetectionDistance = ref(0);
    const distanceOnlyHitDetection = ref(false);

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

    const refreshAvailableNetworks = () => {

    }

    const refreshAvailableBluetoothDevices = () => {

    }

    async function getSettings(): Promise<void> {
        try {
            const response = await fetch(`${API_URL}/settings`, {method: "GET"});

            if(response.ok) {
                const json = await response.json();

                deviceName.value = json["deviceName"];
                devicePassword.value = json["devicePassword"];
                wifiPassword.value = json["wifiPassword"];
                lastValidWifiPassword.value = json["wifiPassword"];
                volume.value = json["volume"];
                metronomeSound.value = json["metronomeSound"];
                hitSound.value = json["hitSound"];
                missSound.value = json["missSound"];
                ledMode.value = json["ledMode"];
                ledBrightness.value = json["ledBrightness"]
                macAddress.value = json["macAddress"];
                isSoundEnabled.value = json["isSoundEnabled"];
                version.value = json["version"];
                vibrationSensorSensitivity.value = json["vibrationSensorSensitivity"];
                ballHitDetectionDistance.value = json["ballHitDetectionDistance"];
                distanceOnlyHitDetection.value = json["distanceOnlyHitDetection"] ?? false;

                // Map ledMode to its corresponding string representation
                const ledModeMapping: { [key: number]: string } = {
                    1: "Ein",
                    2: "Fade",
                    3: "Blitzartig",
                    4: "Turbo",
                    5: "Aus"
                };
                ledModeStr.value = ledModeMapping[json["ledMode"] as number] || "Unknown";


            }

        } catch (error) {
            console.error(error);
        }
    }

    async function saveSettings(): Promise<void> {
        try {
            const state = { ...useSettingsStore().$state };
            state.wifiPassword = isWifiPasswordValid(wifiPassword.value) ? wifiPassword.value : lastValidWifiPassword.value;

            await fetch(`${API_URL}/settings`, {
                method: "POST",
                body: JSON.stringify(state),
            });

            if (isWifiPasswordValid(wifiPassword.value)) {
                lastValidWifiPassword.value = wifiPassword.value;
            }

            if(isSoundEnabled.value) {
                await fetch(`${API_URL}/start`, {
                    method: "POST"
                });
            }
            else {
                await fetch(`${API_URL}/stop`, {
                    method: "POST"
                })
            }
        } catch (error) {
            console.error(error);
        }
    }

    async function restartDevice() : Promise<void> {
        try {
            await fetch(`${API_URL}/restart`, {method: "POST"});
        } catch (error) {
            console.error(error);
        }
    }

    async function factoryResetDevice() : Promise<void> {
        try {
            await fetch(`${API_URL}/factory-reset`, {method: "POST"});
        } catch (error) {
            console.error(error);
        }
    }

    function updateFirmware(firmwareFile: File, onProgress?: (percent: number) => void): void {
        const data = new FormData();
        data.append('file', firmwareFile);

        const xhr = new XMLHttpRequest();
        xhr.open('POST', `${API_URL}/update`);

        xhr.upload.addEventListener('progress', (e) => {
            if (e.lengthComputable && onProgress) {
                onProgress(Math.round((e.loaded / e.total) * 100));
            }
        });

        xhr.send(data);
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
        saveSettings,
        restartDevice,
        factoryResetDevice,
        ledMode,
        ledModeStr,
        ledBrightness,
        isValid,
        isSoundEnabled,
        version,
        updateFirmware
    };
})