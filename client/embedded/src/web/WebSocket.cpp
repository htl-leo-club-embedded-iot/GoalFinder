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
#include "Settings.h"
#include "version.h"

GFWebSocket::GFWebSocket()
    : wsServer(81),
      authAttemptCount(0),
      authTimedOut(false),
      authTimeoutStart(0)
{
    memset(authAttempts, 0, sizeof(authAttempts));
}

GFWebSocket::~GFWebSocket() {}

void GFWebSocket::Begin() {
    wsServer.begin();
    wsServer.onEvent([this](uint8_t clientId, WStype_t type, uint8_t* payload, size_t length) {
        this->OnEvent(clientId, type, payload, length);
    });
    Logger::log("WebSocket", Logger::LogLevel::OK, "WebSocket server started on port 81");
}

void GFWebSocket::Loop() {
    wsServer.loop();
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
            Logger::log("WebSocket", Logger::LogLevel::INFO, "Client %u connected", clientId);
            {
                JsonDocument doc;
                doc["type"] = "connected";
                SendJson(clientId, doc);
            }
            break;
        case WStype_DISCONNECTED:
            Logger::log("WebSocket", Logger::LogLevel::INFO, "Client %u disconnected", clientId);
            break;
        case WStype_TEXT:
            HandleMessage(clientId, payload, length);
            break;
        default:
            break;
    }
}

void GFWebSocket::HandleMessage(uint8_t clientId, uint8_t* payload, size_t length) {
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, payload, length);
    if (err) {
        Logger::log("WebSocket", Logger::LogLevel::WARN, "JSON parse error: %s", err.c_str());
        return;
    }

    const char* type = doc["type"];
    if (!type) return;

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
    } else {
        Logger::log("WebSocket", Logger::LogLevel::WARN, "Unknown message type: %s", type);
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
    data["wifiPassword"] = settings->GetWifiPassword();
    data["devicePassword"] = settings->GetDevicePassword();
    data["vibrationSensorSensitivity"] = settings->GetVibrationSensorSensitivity();
    data["ballHitDetectionDistance"] = settings->GetBallHitDetectionDistance();
    data["distanceOnlyHitDetection"] = settings->GetDistanceOnlyHitDetection();
    data["volume"] = settings->GetVolume();
    data["metronomeSound"] = settings->GetMetronomeSound();
    data["hitSound"] = settings->GetHitSound();
    data["missSound"] = settings->GetMissSound();
    data["ledMode"] = (int)settings->GetLedMode();
    data["ledBrightness"] = settings->GetLedBrightness();
    data["macAddress"] = settings->GetMacAddress();
    data["isSoundEnabled"] = GoalfinderApp::GetInstance()->IsSoundEnabled();
    data["version"] = FIRMWARE_VERSION;
    data["afterHitTimeout"] = settings->GetAfterHitTimeout();

    SendJson(clientId, doc);
}

void GFWebSocket::HandleSetSetting(uint8_t clientId, JsonDocument& doc) {
    const char* key = doc["key"];
    if (!key) return;

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
        response["value"] = settings->GetWifiPassword();
    } else if (strcmp(key, "devicePassword") == 0) {
        settings->SetDevicePassword(doc["value"].as<String>());
        response["value"] = settings->GetDevicePassword();
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
    } else if (strcmp(key, "missSound") == 0) {
        settings->SetMissSound(doc["value"].as<int>());
        response["value"] = settings->GetMissSound();
    } else if (strcmp(key, "ledMode") == 0) {
        settings->SetLedMode((LedMode)doc["value"].as<int>());
        response["value"] = (int)settings->GetLedMode();
    } else if (strcmp(key, "ledBrightness") == 0) {
        settings->SetLedBrightness(doc["value"].as<int>());
        response["value"] = settings->GetLedBrightness();
    } else if (strcmp(key, "afterHitTimeout") == 0) {
        settings->SetAfterHitTimeout(doc["value"].as<int>());
        response["value"] = settings->GetAfterHitTimeout();
    } else if (strcmp(key, "isSoundEnabled") == 0) {
        bool enabled = doc["value"].as<bool>();
        app->SetIsSoundEnabled(enabled);
        response["value"] = enabled;
    } else {
        Logger::log("WebSocket", Logger::LogLevel::WARN, "Unknown setting key: %s", key);
        return;
    }

    Logger::log("WebSocket", Logger::LogLevel::INFO, "Setting '%s' updated", key);
    SendJson(clientId, response);
}

void GFWebSocket::HandleStart(uint8_t clientId) {
    GoalfinderApp::GetInstance()->SetIsSoundEnabled(true);
    JsonDocument doc;
    doc["type"] = "started";
    SendJson(clientId, doc);
    Logger::log("WebSocket", Logger::LogLevel::INFO, "Detection started");
}

void GFWebSocket::HandleStop(uint8_t clientId) {
    GoalfinderApp::GetInstance()->SetIsSoundEnabled(false);
    JsonDocument doc;
    doc["type"] = "stopped";
    SendJson(clientId, doc);
    Logger::log("WebSocket", Logger::LogLevel::INFO, "Detection stopped");
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

    if (authTimedOut) {
        if (now - authTimeoutStart < AUTH_TIMEOUT_MS) {
            response["success"] = false;
            response["error"] = "Too many attempts. Please wait.";
            response["timeout"] = true;
            SendJson(clientId, response);
            return;
        } else {
            authTimedOut = false;
            authAttemptCount = 0;
        }
    }

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
        return;
    }

    authAttempts[authAttemptCount++] = now;

    const char* password = doc["password"];
    if (!password) {
        response["success"] = false;
        response["error"] = "Password required";
        SendJson(clientId, response);
        return;
    }

    String correctPassword = Settings::GetInstance()->GetDevicePassword();
    if (String(password) == correctPassword) {
        response["success"] = true;
    } else {
        response["success"] = false;
        response["error"] = "Invalid password";
    }

    SendJson(clientId, response);
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

