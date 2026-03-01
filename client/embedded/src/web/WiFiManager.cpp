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

#include "WiFiManager.h"
#include <WiFi.h>

const unsigned long WiFiManager::reconnectIntervalMs = 10000;

WiFiManager::WiFiManager()
    : useExternalNW(false),
      connected(false),
      lastReconnectAttemptMs(0),
      wifiMutex(nullptr)
{}

void WiFiManager::Init() {
    wifiMutex = xSemaphoreCreateMutex();
    Settings* settings = Settings::GetInstance();

    if (settings->IsFirstRun()) {
        ApplyDeviceNameByScan();
    }

    useExternalNW = settings->GetUseExternalNW();

    if (useExternalNW) {
        SetupExternalNetwork();
    } else {
        SetupAccessPoint();
    }
}

void WiFiManager::Loop() {
    if (xSemaphoreTake(wifiMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
        if (useExternalNW) {
            MonitorConnection();
        } else {
            dnsServer.processNextRequest();
        }
        xSemaphoreGive(wifiMutex);
    }
}

bool WiFiManager::IsExternalNetwork() const {
    return useExternalNW;
}

void WiFiManager::SetupAccessPoint() {
    Settings* settings = Settings::GetInstance();
    String ssid = settings->GetDeviceName();
    String wifiPassword = settings->GetWifiPassword();

    WiFi.mode(WIFI_AP);

    if (wifiPassword.isEmpty()) {
        WiFi.softAP(ssid);
    } else {
        WiFi.softAP(ssid, wifiPassword.c_str());
    }
    WiFi.setSleep(false);

    dnsServer.start(53, "*", WiFi.softAPIP());
    Logger::log("WiFiManager", Logger::LogLevel::INFO, "AP started - SSID: %s, IP: %s",
        ssid.c_str(), WiFi.softAPIP().toString().c_str());
}

void WiFiManager::SetupExternalNetwork() {
    Settings* settings = Settings::GetInstance();
    String ssid = settings->GetExternalNW_SSID();
    String pwd = settings->GetExternalNW_PWD();

    if (ssid.isEmpty()) {
        Logger::log("WiFiManager", Logger::LogLevel::WARN,
            "External SSID is empty, falling back to AP mode");
        useExternalNW = false;
        SetupAccessPoint();
        return;
    }

    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);

    if (pwd.isEmpty()) {
        WiFi.begin(ssid.c_str());
    } else {
        WiFi.begin(ssid.c_str(), pwd.c_str());
    }

    Logger::log("WiFiManager", Logger::LogLevel::INFO,
        "Connecting to external network '%s'...", ssid.c_str());

    // Wait for initial connection with timeout
    unsigned long startMs = millis();
    const unsigned long timeoutMs = 15000;
    while (WiFi.status() != WL_CONNECTED && (millis() - startMs) < timeoutMs) {
        delay(250);
    }

    if (WiFi.status() == WL_CONNECTED) {
        connected = true;
        Logger::log("WiFiManager", Logger::LogLevel::OK,
            "Connected to '%s', IP: %s",
            ssid.c_str(), WiFi.localIP().toString().c_str());
    } else {
        connected = false;
        Logger::log("WiFiManager", Logger::LogLevel::WARN,
            "Failed to connect to '%s', falling back to AP mode", ssid.c_str());
        WiFi.disconnect();
        useExternalNW = false;
        SetupAccessPoint();
    }
}

void WiFiManager::MonitorConnection() {
    if (WiFi.status() == WL_CONNECTED) {
        if (!connected) {
            connected = true;
            Logger::log("WiFiManager", Logger::LogLevel::OK,
                "Reconnected, IP: %s", WiFi.localIP().toString().c_str());
        }
        return;
    }

    connected = false;
    unsigned long now = millis();
    if ((now - lastReconnectAttemptMs) >= reconnectIntervalMs) {
        lastReconnectAttemptMs = now;
        Logger::log("WiFiManager", Logger::LogLevel::WARN,
            "WiFi disconnected, attempting reconnect...");
        WiFi.reconnect();
    }
}

void WiFiManager::ApplyDeviceNameByScan() {
    Settings* settings = Settings::GetInstance();

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    int n = WiFi.scanNetworks();
    Logger::log("WiFiManager", Logger::LogLevel::INFO, "First run found %d networks", n);

    bool usedNumbers[100] = { false };

    for (int i = 0; i < n; i++) {
        String ssid = WiFi.SSID(i);
        if (ssid.startsWith("GoalFinder")) {
            String numStr = ssid.substring(11);
            int num = numStr.toInt();
            if (num > 0 && num < 100) {
                usedNumbers[num] = true;
                Logger::log("WiFiManager", Logger::LogLevel::INFO,
                    "Found existing device: %s (number %d)", ssid.c_str(), num);
            }
        }
    }
    WiFi.scanDelete();

    int nextNumber = 1;
    for (int i = 1; i < 100; i++) {
        if (!usedNumbers[i]) {
            nextNumber = i;
            break;
        }
    }

    char numberStr[3];
    snprintf(numberStr, sizeof(numberStr), "%02d", nextNumber);
    String deviceName = "GoalFinder " + String(numberStr);

    settings->SetDeviceName(deviceName);
    settings->SetFirstRun(false);
    Logger::log("WiFiManager", Logger::LogLevel::OK,
        "First run assigned device name '%s'", deviceName.c_str());
}
