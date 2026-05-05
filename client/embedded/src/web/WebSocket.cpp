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

#include "WebSocket.h"
#include <GoalfinderApp.h>
#include <mbedtls/sha256.h>
#include "Settings.h"
#include "Version.h"

namespace {
bool ComputeSha256Hex(const String& input, String& output) {
    unsigned char hash[32];
    mbedtls_sha256_context ctx;
    mbedtls_sha256_init(&ctx);

    if (mbedtls_sha256_starts_ret(&ctx, 0) != 0) {
        mbedtls_sha256_free(&ctx);
        return false;
    }

    if (mbedtls_sha256_update_ret(&ctx, reinterpret_cast<const unsigned char*>(input.c_str()), input.length()) != 0) {
        mbedtls_sha256_free(&ctx);
        return false;
    }

    if (mbedtls_sha256_finish_ret(&ctx, hash) != 0) {
        mbedtls_sha256_free(&ctx);
        return false;
    }

    mbedtls_sha256_free(&ctx);

    char hex[65];
    for (size_t i = 0; i < sizeof(hash); ++i) {
        snprintf(&hex[i * 2], 3, "%02x", hash[i]);
    }
    hex[64] = '\0';

    output = String(hex);
    return true;
}
}

GFWebSocket::GFWebSocket()
    : wsServer(81),
      authAttemptCount(0),
      authTimedOut(false),
      authTimeoutStart(0),
      webLogFlag(0)
{
    memset(authAttempts, 0, sizeof(authAttempts));
}

GFWebSocket::~GFWebSocket() {}

void GFWebSocket::Begin() {
    wsServer.begin();
    wsServer.enableHeartbeat(15000, 3000, 2);
    wsServer.onEvent([this](uint8_t clientId, WStype_t type, uint8_t* payload, size_t length) {
        this->OnEvent(clientId, type, payload, length);
    });
    Logger::Log("WebSocket", Logger::LogLevel::OK, "WebSocket server started");
}

void GFWebSocket::Loop() {
    wsServer.loop();
    Logger::DrainWebLogQueue();
}

void GFWebSocket::Task(void* pvParameters) {
    GFWebSocket* ws = static_cast<GFWebSocket*>(pvParameters);
    while (true) {
        ws->Loop();
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

void GFWebSocket::OnEvent(uint8_t clientId, WStype_t type, uint8_t* payload, size_t length) {
    switch (type) {
        case WStype_CONNECTED:
            {
                IPAddress remoteIp = wsServer.remoteIP(clientId);
                Logger::Log("WebSocket", Logger::LogLevel::INFO, "Client %u connected from %s", clientId, remoteIp.toString().c_str());
            }
            {
                JsonDocument doc;
                doc["type"] = "connected";
                SendJson(clientId, doc);
            }
            break;

        case WStype_DISCONNECTED:
            Logger::Log("WebSocket", Logger::LogLevel::INFO, "Client %u disconnected", clientId);
            break;

        case WStype_ERROR:
            if (payload && length > 0) {
                Logger::Log("WebSocket", Logger::LogLevel::ERROR, "Client %u socket error: %.*s", clientId, static_cast<int>(length), reinterpret_cast<const char*>(payload));
            } else {
                Logger::Log("WebSocket", Logger::LogLevel::ERROR, "Client %u socket error", clientId);
            }
            break;

        case WStype_TEXT:
            HandleMessage(clientId, payload, length);
            break;

        default: break;
    }
}

void GFWebSocket::HandleMessage(uint8_t clientId, uint8_t* payload, size_t length) {
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, payload, length);
    if (err) {
        Logger::Log("WebSocket", Logger::LogLevel::WARN, "JSON parse error: %s", err.c_str());
    } else {
        const char* type = doc["type"];
        if (type) {   
            if (strcmp(type, "get_settings") == 0) {
                HandleGetSettings(clientId);
            } else if (strcmp(type, "set") == 0) {
                HandleSetSetting(clientId, doc);
            } else if (strcmp(type, "start") == 0) {
                HandleStart(clientId);
            } else if (strcmp(type, "stop") == 0) {
                HandleStop(clientId);
            } else if (strcmp(type, "restart") == 0) {
                HandleRestart(clientId);
            } else if (strcmp(type, "factory_reset") == 0) {
                HandleFactoryReset(clientId);
            } else if (strcmp(type, "auth") == 0) {
                HandleAuth(clientId, doc);
            } else if (strcmp(type, "is_auth") == 0) {
                HandleIsAuth(clientId);
            } else if (strcmp(type, "ping") == 0) {
                HandlePing(clientId);
            } else if (strcmp(type, "set_web_logging") == 0) {
                HandleSetWebLoggingFlag(clientId, doc);
            } else {
                Logger::Log("WebSocket", Logger::LogLevel::WARN, "Unknown message type: %s", type);
            }
        }
    }
}

void GFWebSocket::SendJson(uint8_t clientId, JsonDocument& doc) {
    String json;
    serializeJson(doc, json);
    wsServer.sendTXT(clientId, json);
}

void GFWebSocket::BroadcastJson(JsonDocument& doc) {
    String json;
    serializeJson(doc, json);
    wsServer.broadcastTXT(json);
}

void GFWebSocket::SendHitEvent() {
    JsonDocument doc;
    doc["type"] = "event";
    doc["event"] = "hit";
    BroadcastJson(doc);
}

void GFWebSocket::SendMissEvent() {
    JsonDocument doc;
    doc["type"] = "event";
    doc["event"] = "miss";
    BroadcastJson(doc);
}

void GFWebSocket::HandleGetSettings(uint8_t clientId) {
    Settings* settings = Settings::GetInstance();
    JsonDocument doc;
    doc["type"] = "settings";
    JsonObject data = doc["data"].to<JsonObject>();

    data["deviceName"] = settings->GetDeviceName();
    data["devicePasswordSet"] = !settings->GetDevicePassword().isEmpty();
    data["wifiPasswordSet"] = !settings->GetWifiPassword().isEmpty();
    data["vibrationSensorSensitivity"] = settings->GetVibrationSensorSensitivity();
    data["ballHitDetectionDistance"] = settings->GetBallHitDetectionDistance();
    data["distanceOnlyHitDetection"] = settings->GetDistanceOnlyHitDetection();
    data["volume"] = settings->GetVolume();
    data["metronomeSound"] = settings->GetMetronomeSound();
    data["hitSound"] = settings->GetHitSound();
    data["missSound"] = settings->GetMissSound();
    data["waitingSound"] = settings->GetWaitingSound();
    data["metSoundDelay"] = settings->GetMetronomeTiming();
    data["ledMode"] = (int)settings->GetLedMode();
    data["ledBrightness"] = settings->GetLedBrightness();
    data["macAddress"] = settings->GetMacAddress();
    data["isSoundEnabled"] = GoalfinderApp::GetInstance()->IsSoundEnabled();
    data["version"] = FIRMWARE_VERSION;
    data["afterHitTimeout"] = settings->GetAfterHitTimeout();
    data["advancedSettingsEnabled"] = settings->AdvancedSettingsEnabled();
    data["DNSEnabled"] = settings->DNSEnabled();
    data["extNW"] = settings->GetUseExternalNW();
    data["extNWSSID"] = settings->GetExternalNW_SSID();
    data["extNWPasswordSet"] = !settings->GetExternalNW_PWD().isEmpty();
    data["extNWUseDHCP"] = settings->GetExternalNWE_UseDHCP();
    data["extNWIP"] = settings->GetExternalNW_IP();
    data["extNWSNM"] = settings->GetExternalNW_SNM();
    data["extNWDFG"] = settings->GetExternalNW_DFG();
    data["extNWDNSIP"] = settings->GetExternalNW_DNSIP();
    data["extNWAuthMode"] = settings->GetExternalNW_AuthMode();
    data["extNWEnterpriseIdentity"] = settings->GetExternalNW_EnterpriseIdentity();
    data["extNWEnterpriseUsername"] = settings->GetExternalNW_EnterpriseUsername();
    data["extNWEnterpriseAnonymousIdentity"] = settings->GetExternalNW_EnterpriseAnonymousIdentity();
    data["extNWEnterprisePasswordSet"] = !settings->GetExternalNW_EnterprisePassword().isEmpty();
    data["extNWEnterprisePhase2Method"] = settings->GetExternalNW_EnterprisePhase2Method();
    data["extNWEnterpriseCaCertificate"] = settings->GetExternalNW_EnterpriseCaCertificate();
    data["extNWEnterpriseClientCertificate"] = settings->GetExternalNW_EnterpriseClientCertificate();
    data["extNWEnterpriseClientPrivateKey"] = settings->GetExternalNW_EnterpriseClientPrivateKey();

    SendJson(clientId, doc);
}

void GFWebSocket::HandleSetSetting(uint8_t clientId, JsonDocument& doc) {
    const char* key = doc["key"];
    if (key) {        
        Settings* settings = Settings::GetInstance();
        GoalfinderApp* app = GoalfinderApp::GetInstance();
        
        JsonDocument response;
        response["type"] = "setting_ack";
        response["key"] = key;
        
        if (strcmp(key, "deviceName") == 0) {
            settings->SetDeviceName(doc["value"].as<String>());
            response["value"] = settings->GetDeviceName();
        } else if (strcmp(key, "wifiPassword") == 0) {
            if (!doc["value"].isNull()) {
                settings->SetWifiPassword(doc["value"].as<String>());
            }
        } else if (strcmp(key, "devicePassword") == 0) {
            if (!doc["value"].isNull()) {
                settings->SetDevicePassword(doc["value"].as<String>());
            }
        } else if (strcmp(key, "vibrationSensorSensitivity") == 0) {
            settings->SetVibrationSensorSensitivity(doc["value"].as<int>());
            response["value"] = settings->GetVibrationSensorSensitivity();
        } else if (strcmp(key, "ballHitDetectionDistance") == 0) {
            settings->SetBallHitDetectionDistance(doc["value"].as<int>());
            response["value"] = settings->GetBallHitDetectionDistance();
        } else if (strcmp(key, "distanceOnlyHitDetection") == 0) {
            settings->SetDistanceOnlyHitDetection(doc["value"].as<bool>());
            response["value"] = settings->GetDistanceOnlyHitDetection();
        } else if (strcmp(key, "volume") == 0) {
            settings->SetVolume(doc["value"].as<int>());
            response["value"] = settings->GetVolume();
        } else if (strcmp(key, "metronomeSound") == 0) {
            settings->SetMetronomeSound(doc["value"].as<int>());
            response["value"] = settings->GetMetronomeSound();
        } else if (strcmp(key, "hitSound") == 0) {
            settings->SetHitSound(doc["value"].as<int>());
            response["value"] = settings->GetHitSound();
        } else if (strcmp(key, "waitingSound") == 0) {
            settings->SetWaitingSound(doc["value"].as<int>());
            response["value"] = settings->GetWaitingSound();
        } else if (strcmp(key, "missSound") == 0) {
            settings->SetMissSound(doc["value"].as<int>());
            response["value"] = settings->GetMissSound();
        } else if (strcmp(key, "metSoundDelay") == 0) {
            settings->SetMetronomeTiming(doc["value"].as<int>());
            response["value"] = settings->GetMetronomeTiming();
        } else if (strcmp(key, "ledMode") == 0) {
            settings->SetLedMode((LedMode)doc["value"].as<int>());
            response["value"] = (int)settings->GetLedMode();
        } else if (strcmp(key, "ledBrightness") == 0) {
            settings->SetLedBrightness(doc["value"].as<int>());
            response["value"] = settings->GetLedBrightness();
        } else if (strcmp(key, "afterHitTimeout") == 0) {
            settings->SetAfterHitTimeout(doc["value"].as<int>());
            response["value"] = settings->GetAfterHitTimeout();
        } else if (strcmp(key, "advancedSettingsEnabled") == 0) {
            settings->SetAdvancedSettingsEnabled(doc["value"].as<bool>());
            response["value"] = settings->AdvancedSettingsEnabled();
        } else if (strcmp(key, "extNW") == 0 || strcmp(key, "useExternalNW") == 0) {
            settings->SetUseExternalNW(doc["value"].as<bool>());
            response["value"] = settings->GetUseExternalNW();
        } else if (strcmp(key, "extNWSSID") == 0) {
            settings->SetExternalNW_SSID(doc["value"].as<String>());
            response["value"] = settings->GetExternalNW_SSID();
        } else if (strcmp(key, "extNWUseDHCP") == 0) {
            settings->SetExternalNWE_UseDHCP(doc["value"].as<bool>());
            response["value"] = settings->GetExternalNWE_UseDHCP();
        } else if (strcmp(key, "extNWIP") == 0) {
            settings->SetExternalNW_IP(doc["value"].as<String>());
            response["value"] = settings->GetExternalNW_IP();
        } else if (strcmp(key, "extNWSNM") == 0) {
            settings->SetExternalNW_SNM(doc["value"].as<String>());
            response["value"] = settings->GetExternalNW_SNM();
        } else if (strcmp(key, "extNWDFG") == 0) {
            settings->SetExternalNW_DFG(doc["value"].as<String>());
            response["value"] = settings->GetExternalNW_DFG();
        } else if (strcmp(key, "extNWDNSIP") == 0) {
            settings->SetExternalNW_DNSIP(doc["value"].as<String>());
            response["value"] = settings->GetExternalNW_DNSIP();
        } else if (strcmp(key, "extNWPWD") == 0) {
            settings->SetExternalNW_PWD(doc["value"].as<String>());
        } else if (strcmp(key, "extNWAuthMode") == 0) {
            settings->SetExternalNW_AuthMode(doc["value"].as<String>());
            response["value"] = settings->GetExternalNW_AuthMode();
        } else if (strcmp(key, "extNWEnterpriseIdentity") == 0) {
            settings->SetExternalNW_EnterpriseIdentity(doc["value"].as<String>());
            response["value"] = settings->GetExternalNW_EnterpriseIdentity();
        } else if (strcmp(key, "extNWEnterpriseUsername") == 0) {
            settings->SetExternalNW_EnterpriseUsername(doc["value"].as<String>());
            response["value"] = settings->GetExternalNW_EnterpriseUsername();
        } else if (strcmp(key, "extNWEnterpriseAnonymousIdentity") == 0) {
            settings->SetExternalNW_EnterpriseAnonymousIdentity(doc["value"].as<String>());
            response["value"] = settings->GetExternalNW_EnterpriseAnonymousIdentity();
        } else if (strcmp(key, "extNWEnterprisePassword") == 0) {
            if (!doc["value"].isNull()) {
                settings->SetExternalNW_EnterprisePassword(doc["value"].as<String>());
            }
        } else if (strcmp(key, "extNWEnterprisePhase2Method") == 0) {
            settings->SetExternalNW_EnterprisePhase2Method(doc["value"].as<String>());
            response["value"] = settings->GetExternalNW_EnterprisePhase2Method();
        } else if (strcmp(key, "extNWEnterpriseCaCertificate") == 0) {
            settings->SetExternalNW_EnterpriseCaCertificate(doc["value"].as<String>());
            response["value"] = settings->GetExternalNW_EnterpriseCaCertificate();
        } else if (strcmp(key, "extNWEnterpriseClientCertificate") == 0) {
            settings->SetExternalNW_EnterpriseClientCertificate(doc["value"].as<String>());
            response["value"] = settings->GetExternalNW_EnterpriseClientCertificate();
        } else if (strcmp(key, "extNWEnterpriseClientPrivateKey") == 0) {
            settings->SetExternalNW_EnterpriseClientPrivateKey(doc["value"].as<String>());
            response["value"] = settings->GetExternalNW_EnterpriseClientPrivateKey();
        } else if (strcmp(key, "DNSEnabled") == 0) {
            settings->SetDNSEnabled(doc["value"].as<bool>());
            response["value"] = settings->DNSEnabled();
        } else if (strcmp(key, "isSoundEnabled") == 0) {
            bool enabled = doc["value"].as<bool>();
            app->SetIsSoundEnabled(enabled);
            response["value"] = enabled;
        } else {
            Logger::Log("WebSocket", Logger::LogLevel::WARN, "Unknown setting key: %s", key);
            return;
        }

        Logger::Log("WebSocket", Logger::LogLevel::INFO, "Setting '%s' updated", key);
        SendJson(clientId, response);
    }
}

void GFWebSocket::SendWebLog(String message) {
    if (webLogFlag) {
        JsonDocument doc;
        doc["type"] = "log";
        doc["message"] = message;
        BroadcastJson(doc);
    }
}

void GFWebSocket::HandleStart(uint8_t clientId) {
    GoalfinderApp::GetInstance()->SetIsSoundEnabled(true);
    JsonDocument doc;
    doc["type"] = "started";
    SendJson(clientId, doc);
    Logger::Log("WebSocket", Logger::LogLevel::INFO, "Detection started");
}

void GFWebSocket::HandleStop(uint8_t clientId) {
    GoalfinderApp::GetInstance()->SetIsSoundEnabled(false);
    JsonDocument doc;
    doc["type"] = "stopped";
    SendJson(clientId, doc);
    Logger::Log("WebSocket", Logger::LogLevel::INFO, "Detection stopped");
}

void GFWebSocket::HandleRestart(uint8_t clientId) {
    JsonDocument doc;
    doc["type"] = "restarting";
    SendJson(clientId, doc);
    delay(200);
    ESP.restart();
}

void GFWebSocket::HandleFactoryReset(uint8_t clientId) {
    JsonDocument doc;
    doc["type"] = "factory_resetting";
    SendJson(clientId, doc);
    Settings::GetInstance()->ResetToDefaults();
}

void GFWebSocket::HandleAuth(uint8_t clientId, JsonDocument& doc) {
    unsigned long now = millis();
    JsonDocument response;
    response["type"] = "auth_result";
    bool timedOut = false;

    // Update timeout status
    if (authTimedOut) {
        if (now - authTimeoutStart < AUTH_TIMEOUT_MS) {
            response["success"] = false;
            response["error"] = "Too many attempts. Please wait.";
            response["timeout"] = true;
            SendJson(clientId, response);
            timedOut = true;
        } else {
            authTimedOut = false;
            authAttemptCount = 0;
        }
    }

    if (!timedOut) {
        int validAttempts = 0;
        for (int i = 0; i < authAttemptCount; i++) {
            if (now - authAttempts[i] < AUTH_TIMEOUT_MS) {
                authAttempts[validAttempts++] = authAttempts[i];
            }
        }
        authAttemptCount = validAttempts;

        if (authAttemptCount >= MAX_AUTH_ATTEMPTS) {
            authTimedOut = true;
            authTimeoutStart = now;
            response["success"] = false;
            response["error"] = "Too many attempts. Please wait.";
            response["timeout"] = true;
            SendJson(clientId, response);
        } else {
            authAttempts[authAttemptCount++] = now;

            const char* passwordHash = doc["passwordHash"];
            if (!passwordHash) {
                response["success"] = false;
                response["error"] = "Password hash required";
                SendJson(clientId, response);
            } else {
                String correctPassword = Settings::GetInstance()->GetDevicePassword();
                String expectedHash;
                if (!ComputeSha256Hex(correctPassword, expectedHash)) {
                    Logger::Log("WebSocket", Logger::LogLevel::ERROR, "Failed to compute password hash");
                    response["success"] = false;
                    response["error"] = "Authentication unavailable";
                    SendJson(clientId, response);
                } else {
                    if (String(passwordHash).equalsIgnoreCase(expectedHash)) {
                        response["success"] = true;
                    } else {
                        response["success"] = false;
                        response["error"] = "Invalid password";
                    }

                    SendJson(clientId, response);
                }
            }
        }
    }
}

void GFWebSocket::HandleIsAuth(uint8_t clientId) {
    JsonDocument doc;
    doc["type"] = "is_auth_result";
    String devicePassword = Settings::GetInstance()->GetDevicePassword();
    doc["isPasswordProtected"] = !devicePassword.isEmpty();
    SendJson(clientId, doc);
}

void GFWebSocket::HandlePing(uint8_t clientId) {
    JsonDocument doc;
    doc["type"] = "pong";
    SendJson(clientId, doc);
}

/**
    Apply flag from ws
    {
        "type": "set_flag",
        "value": true or false
    }
*/
void GFWebSocket::HandleSetWebLoggingFlag(uint8_t clientId, JsonDocument& doc) {
    bool value = doc["value"].as<bool>();
    webLogFlag = value;

    JsonDocument response;
    response["type"] = "set_web_logging_ack";
    response["value"] = value;
    SendJson(clientId, response);
}
