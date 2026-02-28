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

void WiFiManager::Setup() {
    Settings* settings = Settings::GetInstance();

    if (settings->IsFirstRun()) {
        ApplyDeviceNameByScan();
    }

    String ssid = settings->GetDeviceName();
    String wifiPassword = settings->GetWifiPassword();

    WiFi.mode(WIFI_AP);

    if (wifiPassword.isEmpty()) {
        WiFi.softAP(ssid);
    } else {
        WiFi.softAP(ssid, wifiPassword.c_str());
    }
    WiFi.setSleep(false);
    Logger::log("GoalfinderApp", Logger::LogLevel::INFO, "SoftAP IP: %s", WiFi.softAPIP().toString().c_str());
}

void WiFiManager::ApplyDeviceNameByScan() {
    Settings* settings = Settings::GetInstance();

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    int n = WiFi.scanNetworks();
    Logger::log("GoalfinderApp", Logger::LogLevel::INFO, "First run found %d networks", n);

    bool usedNumbers[100] = { false };

    for (int i = 0; i < n; i++) {
        String ssid = WiFi.SSID(i);
        if (ssid.startsWith("GoalFinder")) {
            String numStr = ssid.substring(11);
            int num = numStr.toInt();
            if (num > 0 && num < 100) {
                usedNumbers[num] = true;
                Logger::log("GoalfinderApp", Logger::LogLevel::INFO, "Found existing device: %s (number %d)", ssid.c_str(), num);
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
    Logger::log("GoalfinderApp", Logger::LogLevel::OK, "First run assigned device name '%s'", deviceName.c_str());
}
