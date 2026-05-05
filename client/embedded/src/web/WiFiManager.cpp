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
#include <esp_wifi.h>
#include <esp_err.h>
#include "esp_wpa2.h"

// Helper to do basic PEM-format checks
bool LooksLikePem(const String& s) {
    return s.indexOf("-----BEGIN ") >= 0 && s.indexOf("-----END ") >= 0;
}

namespace {
const char* EXTERNAL_NETWORK_AUTH_MODE_OPEN = "open";
const char* EXTERNAL_NETWORK_AUTH_MODE_WPA2_PERSONAL = "wpa2-personal";
const char* EXTERNAL_NETWORK_AUTH_MODE_WPA2_ENTERPRISE = "wpa2-enterprise";

const char* EXTERNAL_NETWORK_PHASE2_METHOD_AUTO = "auto";
const char* EXTERNAL_NETWORK_PHASE2_METHOD_MSCHAPV2 = "mschapv2";

const char* kDefaultApIp = "192.168.4.1";
const char* kDefaultSubnetMask = "255.255.255.0";

bool ParseIpAddress(const String& rawValue, IPAddress& outAddress) {
    String value = rawValue;
    value.trim();
    return outAddress.fromString(value);
}

void ClearEnterpriseAuthenticationState() {
    esp_wifi_sta_wpa2_ent_clear_identity();
    esp_wifi_sta_wpa2_ent_clear_username();
    esp_wifi_sta_wpa2_ent_clear_password();
    esp_wifi_sta_wpa2_ent_clear_ca_cert();
    esp_wifi_sta_wpa2_ent_clear_cert_key();
    esp_wifi_sta_wpa2_ent_disable();
}

esp_eap_ttls_phase2_types ResolveEnterprisePhase2Method(const String& phase2Method) {
    esp_eap_ttls_phase2_types phase2Type = ESP_EAP_TTLS_PHASE2_EAP;

    if (phase2Method == EXTERNAL_NETWORK_PHASE2_METHOD_MSCHAPV2) {
        phase2Type = ESP_EAP_TTLS_PHASE2_MSCHAPV2;
    }

    return phase2Type;
}

bool ConfigureEnterpriseAuthentication(Settings* settings) {
    bool isConfigured = true;

    String identity = settings->GetExternalNW_EnterpriseIdentity();
    String username = settings->GetExternalNW_EnterpriseUsername();
    String anonymousIdentity = settings->GetExternalNW_EnterpriseAnonymousIdentity();
    String password = settings->GetExternalNW_EnterprisePassword();
    String phase2Method = settings->GetExternalNW_EnterprisePhase2Method();
    String caCertificate = settings->GetExternalNW_EnterpriseCaCertificate();
    String clientCertificate = settings->GetExternalNW_EnterpriseClientCertificate();
    String clientPrivateKey = settings->GetExternalNW_EnterpriseClientPrivateKey();

    identity.trim();
    username.trim();
    anonymousIdentity.trim();
    password.trim();
    phase2Method.trim();
    phase2Method.toLowerCase();
    caCertificate.trim();
    clientCertificate.trim();
    clientPrivateKey.trim();

    String outerIdentity = anonymousIdentity;
    if (outerIdentity.isEmpty()) {
        outerIdentity = identity;
    }
    if (outerIdentity.isEmpty()) {
        outerIdentity = username;
    }

    const bool hasClientCertificate = !clientCertificate.isEmpty();
    const bool hasClientPrivateKey = !clientPrivateKey.isEmpty();

    Logger::Log("WiFiManager", Logger::LogLevel::INFO,
        "Enterprise credential lengths: identity=%d, username=%d, password=%d, ca=%d, cert=%d, key=%d",
        outerIdentity.length(), username.length(), password.length(), caCertificate.length(), clientCertificate.length(), clientPrivateKey.length());

    if (username.isEmpty() || password.isEmpty()) {
        Logger::Log("WiFiManager", Logger::LogLevel::ERROR,
            "Enterprise mode requires username and password (usernameEmpty=%s, passwordEmpty=%s)",
            username.isEmpty() ? "true" : "false",
            password.isEmpty() ? "true" : "false");
        isConfigured = false;
    }

    if (isConfigured && (hasClientCertificate != hasClientPrivateKey)) {
        Logger::Log("WiFiManager", Logger::LogLevel::ERROR,
            "Enterprise client certificate and private key must be provided together");
        isConfigured = false;
    }

    if (isConfigured) {
        ClearEnterpriseAuthenticationState();

        esp_err_t result = esp_wifi_sta_wpa2_ent_set_identity(
            reinterpret_cast<const unsigned char*>(outerIdentity.c_str()),
            outerIdentity.length());
        if (result != ESP_OK) {
            Logger::Log("WiFiManager", Logger::LogLevel::ERROR,
                "Failed to set enterprise identity (err=%d, %s)", static_cast<int>(result), esp_err_to_name(result));
            isConfigured = false;
        }

        if (isConfigured) {
            result = esp_wifi_sta_wpa2_ent_set_username(
                reinterpret_cast<const unsigned char*>(username.c_str()),
                username.length());
            if (result != ESP_OK) {
                Logger::Log("WiFiManager", Logger::LogLevel::ERROR,
                    "Failed to set enterprise username (err=%d, %s)", static_cast<int>(result), esp_err_to_name(result));
                isConfigured = false;
            }
        }

        if (isConfigured) {
            result = esp_wifi_sta_wpa2_ent_set_password(
                reinterpret_cast<const unsigned char*>(password.c_str()),
                password.length());
            if (result != ESP_OK) {
                Logger::Log("WiFiManager", Logger::LogLevel::ERROR,
                    "Failed to set enterprise password (err=%d, %s)", static_cast<int>(result), esp_err_to_name(result));
                isConfigured = false;
            }
        }

        if (isConfigured && !caCertificate.isEmpty()) {
            result = esp_wifi_sta_wpa2_ent_set_ca_cert(
                reinterpret_cast<const unsigned char*>(caCertificate.c_str()),
                caCertificate.length() + 1);
            if (result != ESP_OK) {
                Logger::Log("WiFiManager", Logger::LogLevel::ERROR,
                    "Failed to set enterprise CA certificate (err=%d, %s)", static_cast<int>(result), esp_err_to_name(result));
                isConfigured = false;
            }
        }

        if (isConfigured && hasClientCertificate) {
            result = esp_wifi_sta_wpa2_ent_set_cert_key(
                reinterpret_cast<const unsigned char*>(clientCertificate.c_str()),
                clientCertificate.length() + 1,
                reinterpret_cast<const unsigned char*>(clientPrivateKey.c_str()),
                clientPrivateKey.length() + 1,
                nullptr,
                0);
            if (result != ESP_OK) {
                Logger::Log("WiFiManager", Logger::LogLevel::ERROR,
                    "Failed to set enterprise client cert/key (err=%d, %s)", static_cast<int>(result), esp_err_to_name(result));
                isConfigured = false;
            }
        }

        if (isConfigured && phase2Method != EXTERNAL_NETWORK_PHASE2_METHOD_AUTO) {
            result = esp_wifi_sta_wpa2_ent_set_ttls_phase2_method(
                ResolveEnterprisePhase2Method(phase2Method));
            if (result != ESP_OK) {
                Logger::Log("WiFiManager", Logger::LogLevel::ERROR,
                    "Failed to set enterprise phase2 method (err=%d, %s)", static_cast<int>(result), esp_err_to_name(result));
                isConfigured = false;
            }
        }

        if (isConfigured) {
                if (!caCertificate.isEmpty() && !LooksLikePem(caCertificate)) {
                    Logger::Log("WiFiManager", Logger::LogLevel::ERROR,
                        "CA certificate does not appear to be PEM-formatted");
                    isConfigured = false;
                }
                if (hasClientCertificate && !LooksLikePem(clientCertificate)) {
                    Logger::Log("WiFiManager", Logger::LogLevel::ERROR,
                        "Client certificate does not appear to be PEM-formatted");
                    isConfigured = false;
                }
                if (hasClientPrivateKey && !LooksLikePem(clientPrivateKey)) {
                    Logger::Log("WiFiManager", Logger::LogLevel::ERROR,
                        "Client private key does not appear to be PEM-formatted");
                    isConfigured = false;
                }

                if (isConfigured) {
                    result = esp_wifi_sta_wpa2_ent_enable();
                    if (result != ESP_OK) {
                        Logger::Log("WiFiManager", Logger::LogLevel::ERROR,
                            "Failed to enable enterprise auth (err=%d, %s)", static_cast<int>(result), esp_err_to_name(result));
                        isConfigured = false;
                    }
                }
        }
    }

    return isConfigured;
}
}

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

    // Register lightweight WiFi event handler for additional diagnostics
    WiFi.onEvent([this](WiFiEvent_t event, WiFiEventInfo_t info) {
        switch (event) {
#if defined(ARDUINO_EVENT_WIFI_STA_DISCONNECTED)
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            Logger::Log("WiFiManager", Logger::LogLevel::WARN,
                "WiFi event: STA_DISCONNECTED (reason=%d)", info.disconnected.reason);
            break;
#endif
#if defined(ARDUINO_EVENT_WIFI_STA_CONNECTED)
        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
            Logger::Log("WiFiManager", Logger::LogLevel::INFO,
                "WiFi event: STA_CONNECTED");
            break;
#endif
#if defined(ARDUINO_EVENT_WIFI_STA_GOT_IP)
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            Logger::Log("WiFiManager", Logger::LogLevel::OK,
                "WiFi event: GOT_IP %s", WiFi.localIP().toString().c_str());
            break;
#endif
        default:
            break;
        }
    });

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
    String configuredIp = settings->GetDeviceIpAddress();
    String configuredMask = settings->GetSubnetMask();

    IPAddress apIp;
    if (!apIp.fromString(configuredIp)) {
        Logger::Log("WiFiManager", Logger::LogLevel::WARN,
            "Invalid device IP '%s'. Falling back to %s", configuredIp.c_str(), kDefaultApIp);
        configuredIp = kDefaultApIp;
        apIp.fromString(configuredIp);
        settings->SetDeviceIpAddress(configuredIp);
    }

    IPAddress subnetMask;
    if (!subnetMask.fromString(configuredMask)) {
        Logger::Log("WiFiManager", Logger::LogLevel::WARN,
            "Invalid subnet mask '%s'. Falling back to %s", configuredMask.c_str(), kDefaultSubnetMask);
        configuredMask = kDefaultSubnetMask;
        subnetMask.fromString(configuredMask);
        settings->SetSubnetMask(configuredMask);
    }

    ClearEnterpriseAuthenticationState();
    WiFi.mode(WIFI_AP);

    if (!WiFi.softAPConfig(apIp, apIp, subnetMask)) {
        Logger::Log("WiFiManager", Logger::LogLevel::WARN,
            "Failed to apply AP config IP=%s Mask=%s", configuredIp.c_str(), configuredMask.c_str());
    }

    if (wifiPassword.isEmpty()) {
        WiFi.softAP(ssid);
    } else {
        WiFi.softAP(ssid, wifiPassword.c_str());
    }
    WiFi.setSleep(false);

    Logger::Log("WiFiManager", Logger::LogLevel::OK, "AP started - SSID: %s, IP: %s",
        ssid.c_str(), WiFi.softAPIP().toString().c_str());
}

void WiFiManager::SetupExternalNetwork() {
    Settings* settings = Settings::GetInstance();
    String ssid = settings->GetExternalNW_SSID();
    String pwd = settings->GetExternalNW_PWD();
    String authMode = settings->GetExternalNW_AuthMode();
    const bool useDhcp = settings->GetExternalNWE_UseDHCP();
    bool fallbackToAccessPoint = false;

    authMode.trim();
    authMode.toLowerCase();
    if (
        authMode != EXTERNAL_NETWORK_AUTH_MODE_OPEN &&
        authMode != EXTERNAL_NETWORK_AUTH_MODE_WPA2_PERSONAL &&
        authMode != EXTERNAL_NETWORK_AUTH_MODE_WPA2_ENTERPRISE
    ) {
        Logger::Log("WiFiManager", Logger::LogLevel::ERROR,
            "Unknown external auth mode '%s', defaulting to WPA2 personal", authMode.c_str());
        authMode = EXTERNAL_NETWORK_AUTH_MODE_WPA2_PERSONAL;
    }

    if (ssid.isEmpty()) {
        Logger::Log("WiFiManager", Logger::LogLevel::ERROR,
            "External SSID is empty, falling back to AP mode");
        fallbackToAccessPoint = true;
    } else {
        WiFi.mode(WIFI_STA);
        WiFi.setSleep(false);

        if (useDhcp) {
            // Rely on the stack's default DHCP behavior instead of attempting
            // to "reset" DHCP via WiFi.config(). Calling WiFi.config with
            // zero addresses may fail on some cores and cause spurious fallbacks.
            Logger::Log("WiFiManager", Logger::LogLevel::INFO,
                "Using DHCP for external network '%s'", ssid.c_str());
        } else {
            IPAddress staticIp;
            IPAddress subnetMask;
            IPAddress defaultGateway;
            IPAddress dnsServer;

            const bool hasStaticIp = ParseIpAddress(settings->GetExternalNW_IP(), staticIp);
            const bool hasSubnetMask = ParseIpAddress(settings->GetExternalNW_SNM(), subnetMask);
            const bool hasDefaultGateway = ParseIpAddress(settings->GetExternalNW_DFG(), defaultGateway);
            const bool hasDnsServer = ParseIpAddress(settings->GetExternalNW_DNSIP(), dnsServer);

            if (!hasStaticIp || !hasSubnetMask || !hasDefaultGateway || !hasDnsServer) {
                Logger::Log("WiFiManager", Logger::LogLevel::ERROR,
                    "Invalid static external network config (IP/Gateway/Subnet/DNS), falling back to AP mode");
                fallbackToAccessPoint = true;
            } else if (!WiFi.config(staticIp, defaultGateway, subnetMask, dnsServer)) {
                Logger::Log("WiFiManager", Logger::LogLevel::ERROR,
                    "Failed to apply static external network config, falling back to AP mode");
                fallbackToAccessPoint = true;
            } else {
                Logger::Log("WiFiManager", Logger::LogLevel::INFO,
                    "Using static external network config - IP: %s, Gateway: %s, Subnet: %s, DNS: %s",
                    staticIp.toString().c_str(),
                    defaultGateway.toString().c_str(),
                    subnetMask.toString().c_str(),
                    dnsServer.toString().c_str());
            }
        }

        if (!fallbackToAccessPoint) {
            // Pre-flight scan: attempt to find SSID and log channel/RSSI/encryption.
            int scanCount = WiFi.scanNetworks();
            Logger::Log("WiFiManager", Logger::LogLevel::INFO,
                "Pre-connect scan found %d networks", scanCount);
            int foundIndex = -1;
            for (int i = 0; i < scanCount; ++i) {
                if (WiFi.SSID(i) == ssid) {
                    foundIndex = i;
                    break;
                }
            }
            if (foundIndex >= 0) {
                int ch = WiFi.channel(foundIndex);
                int rssi = WiFi.RSSI(foundIndex);
                int enc = WiFi.encryptionType(foundIndex);
                Logger::Log("WiFiManager", Logger::LogLevel::OK,
                    "Found SSID '%s' in scan: channel=%d, rssi=%d, enc=%d",
                    ssid.c_str(), ch, rssi, enc);
                // Channels > 14 indicate 5GHz band on typical WiFi chips.
                if (ch > 14) {
                    Logger::Log("WiFiManager", Logger::LogLevel::ERROR,
                        "Target SSID appears to be on 5GHz channel %d; device requires 2.4GHz or a dual-band SSID", ch);
                    fallbackToAccessPoint = true;
                }
            } else {
                Logger::Log("WiFiManager", Logger::LogLevel::ERROR,
                    "Target SSID '%s' not found in scan results; proceeding (may be hidden)", ssid.c_str());
            }
            WiFi.scanDelete();

            if (!fallbackToAccessPoint) {
                bool enterpriseMode = (authMode == EXTERNAL_NETWORK_AUTH_MODE_WPA2_ENTERPRISE);
                bool usePassword = (authMode == EXTERNAL_NETWORK_AUTH_MODE_WPA2_PERSONAL);

                if (enterpriseMode) {
                    if (!ConfigureEnterpriseAuthentication(settings)) {
                        Logger::Log("WiFiManager", Logger::LogLevel::ERROR,
                            "Failed to configure enterprise authentication, falling back to AP mode");
                        fallbackToAccessPoint = true;
                    }
                } else if (!usePassword && authMode != EXTERNAL_NETWORK_AUTH_MODE_OPEN) {
                    // Unexpected auth mode after normalization - treat as personal with password
                    usePassword = true;
                }

                if (!fallbackToAccessPoint) {
                    const int maxAttempts = enterpriseMode ? 3 : 2;
                    const unsigned long attemptTimeoutMs = enterpriseMode ? 60000UL : 15000UL;
                    for (int attempt = 1; attempt <= maxAttempts && !connected; ++attempt) {
                        // (Re)start the connection for each attempt so auth is re-negotiated.
                        if (authMode == EXTERNAL_NETWORK_AUTH_MODE_OPEN) {
                            ClearEnterpriseAuthenticationState();
                            WiFi.begin(ssid.c_str());
                            Logger::Log("WiFiManager", Logger::LogLevel::INFO,
                                "Connecting to open external network '%s' (attempt %d/%d)...", ssid.c_str(), attempt, maxAttempts);
                        } else if (enterpriseMode) {
                            WiFi.begin(ssid.c_str());
                            Logger::Log("WiFiManager", Logger::LogLevel::INFO,
                                "Connecting to WPA2 enterprise network '%s' (attempt %d/%d)...", ssid.c_str(), attempt, maxAttempts);
                        } else {
                            if (pwd.isEmpty()) {
                                Logger::Log("WiFiManager", Logger::LogLevel::WARN,
                                    "WPA2 personal mode requires a password, falling back to AP mode");
                                fallbackToAccessPoint = true;
                                break;
                            }
                            ClearEnterpriseAuthenticationState();
                            WiFi.begin(ssid.c_str(), pwd.c_str());
                            Logger::Log("WiFiManager", Logger::LogLevel::INFO,
                                "Connecting to WPA2 personal network '%s' (attempt %d/%d)...", ssid.c_str(), attempt, maxAttempts);
                        }

                        unsigned long startMs = millis();
                        while (WiFi.status() != WL_CONNECTED && (millis() - startMs) < attemptTimeoutMs) {
                            delay(250);
                        }

                        if (WiFi.status() == WL_CONNECTED) {
                            connected = true;
                            Logger::Log("WiFiManager", Logger::LogLevel::OK,
                                "Connected to '%s', IP: %s",
                                ssid.c_str(), WiFi.localIP().toString().c_str());
                            break;
                        } else {
                            Logger::Log("WiFiManager", Logger::LogLevel::WARN,
                                "Attempt %d/%d to connect to '%s' failed", attempt, maxAttempts, ssid.c_str());
                            WiFi.disconnect();
                            if (attempt < maxAttempts) {
                                vTaskDelay(pdMS_TO_TICKS(1000UL * attempt));
                            } else {
                                fallbackToAccessPoint = true;
                            }
                        }
                    }
                }
            }
        }
    }

    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);

    if (pwd.isEmpty()) {
        WiFi.begin(ssid.c_str());
    } else {
        WiFi.begin(ssid.c_str(), pwd.c_str());
    }

    Logger::Log("WiFiManager", Logger::LogLevel::INFO,
        "Connecting to external network '%s'...", ssid.c_str());

    // Wait for initial connection with timeout
    unsigned long startMs = millis();
    const unsigned long timeoutMs = 15000;
    while (WiFi.status() != WL_CONNECTED && (millis() - startMs) < timeoutMs) {
        delay(250);
    }

    if (WiFi.status() == WL_CONNECTED) {
        connected = true;
        Logger::Log("WiFiManager", Logger::LogLevel::OK,
            "Connected to '%s', IP: %s",
            ssid.c_str(), WiFi.localIP().toString().c_str());
    } else {
        connected = false;
        Logger::Log("WiFiManager", Logger::LogLevel::WARN,
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
            Logger::Log("WiFiManager", Logger::LogLevel::OK,
                "Reconnected, IP: %s", WiFi.localIP().toString().c_str());
        }
        return;
    }

    connected = false;
    unsigned long now = millis();
    if ((now - lastReconnectAttemptMs) >= reconnectIntervalMs) {
        lastReconnectAttemptMs = now;
        Logger::Log("WiFiManager", Logger::LogLevel::WARN,
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
    Logger::Log("WiFiManager", Logger::LogLevel::INFO, "First run found %d networks", n);

    bool usedNumbers[100] = { false };

    for (int i = 0; i < n; i++) {
        String ssid = WiFi.SSID(i);
        if (ssid.startsWith("GoalFinder")) {
            String numStr = ssid.substring(11);
            int num = numStr.toInt();
            if (num > 0 && num < 100) {
                usedNumbers[num] = true;
                Logger::Log("WiFiManager", Logger::LogLevel::INFO,
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
    Logger::Log("WiFiManager", Logger::LogLevel::OK,
        "First run assigned device name '%s'", deviceName.c_str());
}
