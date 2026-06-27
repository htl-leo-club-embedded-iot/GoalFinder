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
#include "DevicePrivateKey.h"
#include <mbedtls/pk.h>
#include <mbedtls/rsa.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/entropy.h>
#include <mbedtls/base64.h>

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

// Try to decrypt a value that was encoded as "RSA:<base64>" using the
// device private key. Returns true on success and sets `output` to the
// decrypted plaintext. Returns false if the input is not encrypted or
// decryption failed.
bool TryDecryptIfEncrypted(const String& input, String& output) {
    bool result = false;
    const char* prefix = "RSA:";
    mbedtls_pk_context pk;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_pk_init(&pk);
    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&ctr_drbg);

    auto cleanup = [&pk, &ctr_drbg, &entropy]() {   mbedtls_pk_free(&pk);
        mbedtls_ctr_drbg_free(&ctr_drbg);
        mbedtls_entropy_free(&entropy);
    };

    if (input.startsWith(prefix)) {
        String b64 = input.substring(strlen(prefix));

        // Base64 decode
        unsigned char encBuf[512];
        size_t encLen = 0;
        int r = mbedtls_base64_decode(encBuf, sizeof(encBuf), &encLen, (const unsigned char*)b64.c_str(), b64.length());
        if (r == 0) {
            const char* pers = "gf_ws_rsa";

            if (mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, (const unsigned char*)pers, strlen(pers)) == 0
                && mbedtls_pk_parse_key(&pk, (const unsigned char*)DEVICE_PRIVATE_KEY_PEM, strlen(DEVICE_PRIVATE_KEY_PEM) + 1, NULL, 0) == 0) {

                // Prefer RSA OAEP decryption when available
                if (mbedtls_pk_get_type(&pk) == MBEDTLS_PK_RSA) {
                    mbedtls_rsa_context* rsa = mbedtls_pk_rsa(pk);
                    mbedtls_rsa_set_padding(rsa, MBEDTLS_RSA_PKCS_V21, MBEDTLS_MD_SHA256);

                    unsigned char outBuf[512];
                    size_t outLen = 0;
                    int ret = mbedtls_rsa_rsaes_oaep_decrypt(rsa, mbedtls_ctr_drbg_random, &ctr_drbg,
                                                            MBEDTLS_RSA_PRIVATE, NULL, 0, &outLen,
                                                            encBuf, outBuf, sizeof(outBuf));
                    if (ret == 0) {
                        output = String((const char*)outBuf, outLen);
                        result = true;
                    }
                } else {
                    // Fallback to generic pk_decrypt
                    unsigned char outBuf[512];
                    size_t outLen = 0;
                    int ret = mbedtls_pk_decrypt(&pk, encBuf, encLen, outBuf, &outLen, sizeof(outBuf), mbedtls_ctr_drbg_random, &ctr_drbg);
                    if (ret == 0) {
                        output = String((const char*)outBuf, outLen);
                        result = true;
                    }
                }
            }
        }
    }

    cleanup();
    return result;
}

GFWebSocket::GFWebSocket()
    : wsServer(81),
      authAttemptCount(0),
      authTimedOut(false),
      authTimeoutStart(0),
      webLogFlag(0)
{
    memset(authAttempts, 0, sizeof(authAttempts));
    for (int i = 0; i < MAX_WS_CLIENTS; i++) {
        _clients[i].active = false;
        _clients[i].isAuthenticated = false;
        _clients[i].isHub = false;
        _clients[i].sourceType = SourceType::WA_NO_AUTH;
    }
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
                InitClientInfo(clientId);
            }
            {
                JsonDocument doc;
                doc["type"] = "connected";
                SendJson(clientId, doc);
            }
            break;

        case WStype_DISCONNECTED:
            Logger::Log("WebSocket", Logger::LogLevel::INFO, "Client %u disconnected", clientId);
            ClearClientInfo(clientId);
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
            SourceType source = GetSourceType(clientId);
            if (!CheckPermission(source, type)) {
                SendPermissionDenied(clientId, type);
            } else if (strcmp(type, "get_settings") == 0) {
                HandleGetSettings(clientId);
            } else if (strcmp(type, "set_settings") == 0) {
                HandleSetSetting(clientId, doc);
            } else if (strcmp(type, "get_game") == 0) {
                HandleGetGame(clientId);
            } else if (strcmp(type, "set_game") == 0) {
                HandleSetGame(clientId, doc);
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
            } else if (strcmp(type, "identify") == 0) {
                HandleIdentify(clientId, doc);
            } else {
                Logger::Log("WebSocket", Logger::LogLevel::WARN, "Unknown message type: %s", type);
            }
        }
    }
}

void GFWebSocket::SendJson(uint8_t clientId, JsonDocument& doc) {
    String json;
    doc["sourceType"] = SourceTypeToString(GetSourceType(clientId));
    serializeJson(doc, json);
    wsServer.sendTXT(clientId, json);
}

void GFWebSocket::SendJsonToAll(JsonDocument& doc) {
    String baseJson;
    serializeJson(doc, baseJson);

    for (int i = 0; i < MAX_WS_CLIENTS; i++) {
        if (_clients[i].active) {
            String clientJson = baseJson.substring(0, baseJson.length() - 1);
            clientJson += ",\"sourceType\":\"";
            clientJson += SourceTypeToString(_clients[i].sourceType);
            clientJson += "\"}";
            wsServer.sendTXT(i, clientJson);
        }
    }
}

void GFWebSocket::SendHitEvent() {
    JsonDocument doc;
    doc["type"] = "event";
    doc["event"] = "hit";
    SendJsonToAll(doc);
}

void GFWebSocket::SendMissEvent() {
    JsonDocument doc;
    doc["type"] = "event";
    doc["event"] = "miss";
    SendJsonToAll(doc);
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
    data["isSoundEnabled"] = GoalFinderApp::GetInstance()->IsSoundEnabled();
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

namespace {
const char* ModeToKey(GameMode mode) {
    const char* result = "";
    if (mode == GameMode::FREE_PLAY) {
        result = "free_play";
    } else if (mode == GameMode::TIMED_SHOTS) {
        result = "timed_shots";
    } else if (mode == GameMode::BOARD_HITS) {
        result = "board_hits";
    }
    return result;
}

GameMode KeyToMode(const char* key) {
    GameMode result = GameMode::FREE_PLAY;
    if (strcmp(key, "timed_shots") == 0) {
        result = GameMode::TIMED_SHOTS;
    } else if (strcmp(key, "board_hits") == 0) {
        result = GameMode::BOARD_HITS;
    }
    return result;
}
}

void GFWebSocket::HandleGetGame(uint8_t clientId) {
    JsonDocument doc;
    doc["type"] = "game_state";
    JsonObject data = doc["data"].to<JsonObject>();
    data["isSoundEnabled"] = GoalFinderApp::GetInstance()->IsSoundEnabled();
    data["isDetecting"] = false;

    Settings* settings = Settings::GetInstance();
    GamePreset (*presets)[Settings::PRESETS_PER_MODE] = settings->GetGamePresets();
    JsonObject presetsObj = data["presets"].to<JsonObject>();

    for (int m = 0; m < Settings::GAME_MODE_COUNT; m++) {
        GameMode mode = static_cast<GameMode>(m);
        JsonArray arr = presetsObj[ModeToKey(mode)].to<JsonArray>();
        for (int p = 0; p < Settings::PRESETS_PER_MODE; p++) {
            JsonObject presetObj = arr.add<JsonObject>();
            presetObj["name"] = presets[m][p].name;
            presetObj["rounds"] = presets[m][p].rounds;
            presetObj["timePerTurn"] = presets[m][p].timePerTurn;
        }
    }

    PlayerSet* playerSets = settings->GetPlayerSets();
    JsonArray playerSetsArr = data["playerSets"].to<JsonArray>();
    for (int i = 0; i < Settings::PLAYER_SET_COUNT; i++) {
        JsonObject setObj = playerSetsArr.add<JsonObject>();
        setObj["name"] = playerSets[i].name;
        JsonArray playersArr = setObj["players"].to<JsonArray>();
        for (int j = 0; j < Settings::PLAYERS_PER_SET; j++) {
            if (playerSets[i].players[j][0] != '\0') {
                playersArr.add(playerSets[i].players[j]);
            }
        }
    }

    SendJson(clientId, doc);
}

void GFWebSocket::HandleSetGame(uint8_t clientId, JsonDocument& doc) {
    JsonDocument response;
    response["type"] = "game_ack";

    if (!doc["data"].isNull()) {
        JsonObject data = doc["data"];

        if (data["isSoundEnabled"].is<bool>()) {
            GoalFinderApp::GetInstance()->SetIsSoundEnabled(data["isSoundEnabled"].as<bool>());
        }

        if (data["presets"].is<JsonObject>()) {
            JsonObject presetsObj = data["presets"];
            Settings* settings = Settings::GetInstance();

            for (int m = 0; m < Settings::GAME_MODE_COUNT; m++) {
                GameMode mode = static_cast<GameMode>(m);
                const char* key = ModeToKey(mode);

                if (presetsObj[key].is<JsonArray>()) {
                    JsonArray arr = presetsObj[key].as<JsonArray>();
                    int count = arr.size();
                    if (count > Settings::PRESETS_PER_MODE) {
                        count = Settings::PRESETS_PER_MODE;
                    }

                    GamePreset modePresets[Settings::PRESETS_PER_MODE];

                    for (int p = 0; p < count; p++) {
                        JsonObject presetObj = arr[p];
                        const char* name = presetObj["name"] | "";
                        size_t nameLen = strlen(name);
                        if (nameLen > 16) {
                            nameLen = 16;
                        }
                        memcpy(modePresets[p].name, name, nameLen);
                        modePresets[p].name[nameLen] = '\0';
                        modePresets[p].rounds = presetObj["rounds"] | 0;
                        modePresets[p].timePerTurn = presetObj["timePerTurn"] | 0;
                    }

                    for (int p = count; p < Settings::PRESETS_PER_MODE; p++) {
                        modePresets[p].name[0] = '\0';
                        modePresets[p].rounds = 0;
                        modePresets[p].timePerTurn = 0;
                    }

                    settings->SetAllGamePresets(mode, modePresets);
                }
            }
        }

        if (data["playerSets"].is<JsonArray>()) {
            JsonArray playerSetsArr = data["playerSets"];
            Settings* settings = Settings::GetInstance();
            int setCount = playerSetsArr.size();
            if (setCount > Settings::PLAYER_SET_COUNT) {
                setCount = Settings::PLAYER_SET_COUNT;
            }

            PlayerSet playerSets[Settings::PLAYER_SET_COUNT];

            for (int i = 0; i < setCount; i++) {
                JsonObject setObj = playerSetsArr[i];
                const char* setName = setObj["name"] | "";
                size_t setNameLen = strlen(setName);
                if (setNameLen > 16) {
                    setNameLen = 16;
                }
                memcpy(playerSets[i].name, setName, setNameLen);
                playerSets[i].name[setNameLen] = '\0';

                for (int j = 0; j < Settings::PLAYERS_PER_SET; j++) {
                    playerSets[i].players[j][0] = '\0';
                }

                if (setObj["players"].is<JsonArray>()) {
                    JsonArray playersArr = setObj["players"];
                    int playerCount = playersArr.size();
                    if (playerCount > Settings::PLAYERS_PER_SET) {
                        playerCount = Settings::PLAYERS_PER_SET;
                    }
                    for (int j = 0; j < playerCount; j++) {
                        const char* playerName = playersArr[j] | "";
                        size_t playerNameLen = strlen(playerName);
                        if (playerNameLen > 16) {
                            playerNameLen = 16;
                        }
                        memcpy(playerSets[i].players[j], playerName, playerNameLen);
                        playerSets[i].players[j][playerNameLen] = '\0';
                    }
                }
            }

            for (int i = setCount; i < Settings::PLAYER_SET_COUNT; i++) {
                playerSets[i].name[0] = '\0';
                for (int j = 0; j < Settings::PLAYERS_PER_SET; j++) {
                    playerSets[i].players[j][0] = '\0';
                }
            }

            settings->SetAllPlayerSets(playerSets);
        }
    }

    SendJson(clientId, response);
}

void GFWebSocket::HandleSetSetting(uint8_t clientId, JsonDocument& doc) {
    const char* key = doc["key"];
    if (key) {        
        Settings* settings = Settings::GetInstance();
        GoalFinderApp* app = GoalFinderApp::GetInstance();
        
        JsonDocument response;
        response["type"] = "setting_ack";
        response["key"] = key;
        
        if (strcmp(key, "deviceName") == 0) {
            settings->SetDeviceName(doc["value"].as<String>());
            response["value"] = settings->GetDeviceName();
        } else if (strcmp(key, "wifiPassword") == 0) {
            if (!doc["value"].isNull()) {
                String v = doc["value"].as<String>();
                String dec;
                if (TryDecryptIfEncrypted(v, dec)) {
                    settings->SetWifiPassword(dec);
                } else {
                    settings->SetWifiPassword(v);
                }
            }
        } else if (strcmp(key, "devicePassword") == 0) {
            if (!doc["value"].isNull()) {
                String v = doc["value"].as<String>();
                String dec;
                if (TryDecryptIfEncrypted(v, dec)) {
                    settings->SetDevicePassword(dec);
                } else {
                    settings->SetDevicePassword(v);
                }
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
            {
                String v = doc["value"].as<String>();
                String dec;
                if (TryDecryptIfEncrypted(v, dec)) {
                    settings->SetExternalNW_PWD(dec);
                } else {
                    settings->SetExternalNW_PWD(v);
                }
            }
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
                String v = doc["value"].as<String>();
                String dec;
                if (TryDecryptIfEncrypted(v, dec)) {
                    settings->SetExternalNW_EnterprisePassword(dec);
                } else {
                    settings->SetExternalNW_EnterprisePassword(v);
                }
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
            {
                String v = doc["value"].as<String>();
                String dec;
                if (TryDecryptIfEncrypted(v, dec)) {
                    settings->SetExternalNW_EnterpriseClientPrivateKey(dec);
                } else {
                    settings->SetExternalNW_EnterpriseClientPrivateKey(v);
                }
                response["value"] = settings->GetExternalNW_EnterpriseClientPrivateKey();
            }
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
        SendJsonToAll(doc);
    }
}

void GFWebSocket::HandleStart(uint8_t clientId) {
    GoalFinderApp::GetInstance()->SetIsSoundEnabled(true);
    JsonDocument doc;
    doc["type"] = "started";
    SendJson(clientId, doc);
    Logger::Log("WebSocket", Logger::LogLevel::INFO, "Detection started");
}

void GFWebSocket::HandleStop(uint8_t clientId) {
    GoalFinderApp::GetInstance()->SetIsSoundEnabled(false);
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
                        _clients[clientId].isAuthenticated = true;
                        _clients[clientId].sourceType = SourceType::WA_AUTH;
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

void GFWebSocket::InitClientInfo(uint8_t clientId) {
    if (clientId < MAX_WS_CLIENTS) {
        _clients[clientId].active = true;
        _clients[clientId].isAuthenticated = false;
        _clients[clientId].isHub = false;
        _clients[clientId].sourceType = SourceType::WA_NO_AUTH;
    }
}

void GFWebSocket::ClearClientInfo(uint8_t clientId) {
    if (clientId < MAX_WS_CLIENTS) {
        _clients[clientId].active = false;
        _clients[clientId].isAuthenticated = false;
        _clients[clientId].isHub = false;
        _clients[clientId].sourceType = SourceType::WA_NO_AUTH;
    }
}

SourceType GFWebSocket::GetSourceType(uint8_t clientId) const {
    SourceType result = SourceType::WA_NO_AUTH;
    if (clientId < MAX_WS_CLIENTS && _clients[clientId].active) {
        result = _clients[clientId].sourceType;
    }
    return result;
}

const char* GFWebSocket::SourceTypeToString(SourceType st) const {
    const char* result = "wa-no-auth";
    if (st == SourceType::WA_AUTH) {
        result = "wa-auth";
    } else if (st == SourceType::HUB) {
        result = "hub";
    }
    return result;
}

SourceType GFWebSocket::StringToSourceType(const char* str) const {
    SourceType result = SourceType::WA_NO_AUTH;
    if (str) {
        if (strcmp(str, "wa-auth") == 0) {
            result = SourceType::WA_AUTH;
        } else if (strcmp(str, "hub") == 0) {
            result = SourceType::HUB;
        }
    }
    return result;
}

bool GFWebSocket::CheckPermission(SourceType source, const char* messageType) const {
    bool allowed = false;

    if (strcmp(messageType, "get_settings") == 0 ||
        strcmp(messageType, "get_game") == 0 ||
        strcmp(messageType, "is_auth") == 0 ||
        strcmp(messageType, "auth") == 0 ||
        strcmp(messageType, "ping") == 0) {
        allowed = true;
    } else if (strcmp(messageType, "set_settings") == 0 ||
               strcmp(messageType, "set_game") == 0 ||
               strcmp(messageType, "set_web_logging") == 0 ||
               strcmp(messageType, "identify") == 0 ||
               strcmp(messageType, "start") == 0 ||
               strcmp(messageType, "stop") == 0 ||
               strcmp(messageType, "restart") == 0 ||
               strcmp(messageType, "factory_reset") == 0) {
        allowed = (source >= SourceType::WA_AUTH);
    }

    return allowed;
}

void GFWebSocket::SendPermissionDenied(uint8_t clientId, const char* messageType) {
    JsonDocument doc;
    doc["type"] = "error";
    doc["error"] = "permission_denied";
    doc["messageType"] = messageType;
    doc["sourceType"] = SourceTypeToString(GetSourceType(clientId));
    SendJson(clientId, doc);
}

void GFWebSocket::HandleIdentify(uint8_t clientId, JsonDocument& doc) {
    JsonDocument response;
    response["type"] = "identify_ack";

    if (_clients[clientId].isAuthenticated) {
        const char* role = doc["role"];
        if (role) {
            if (strcmp(role, "hub") == 0) {
                _clients[clientId].isHub = true;
                _clients[clientId].sourceType = SourceType::HUB;
                response["role"] = "hub";
            } else {
                response["role"] = "unknown";
                response["error"] = "Unrecognized role";
            }
        } else {
            response["error"] = "Role required";
        }
    } else {
        response["error"] = "Not authenticated";
    }

    SendJson(clientId, response);
}
