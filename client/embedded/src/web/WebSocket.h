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

#pragma once

#ifndef ___WEB_SOCKET_H
#define ___WEB_SOCKET_H

#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include "util/Logger.h"

// Forward declare
class GoalfinderApp;

class GFWebSocket {
public:
    GFWebSocket();
    ~GFWebSocket();

    /** Initialize and start the WebSocket server on port 81 */
    void Begin();

    /** Process WebSocket events - call from FreeRTOS task loop */
    void Loop();

    /** Send a hit event to all connected clients */
    void SendHitEvent();

    /** Send a miss event to all connected clients */
    void SendMissEvent();

    /** FreeRTOS task entry point */
    static void Task(void* pvParameters);

private:
    WebSocketsServer wsServer;

    // Rate limiting for auth
    static const int MAX_AUTH_ATTEMPTS = 5;
    static const unsigned long AUTH_TIMEOUT_MS = 60000;
    unsigned long authAttempts[MAX_AUTH_ATTEMPTS];
    int authAttemptCount;
    bool authTimedOut;
    unsigned long authTimeoutStart;

    /** WebSocket event handler */
    void OnEvent(uint8_t clientId, WStype_t type, uint8_t* payload, size_t length);

    /** Process an incoming JSON message */
    void HandleMessage(uint8_t clientId, uint8_t* payload, size_t length);

    /** Send JSON message to a specific client */
    void SendJson(uint8_t clientId, JsonDocument& doc);

    /** Broadcast JSON message to all clients */
    void BroadcastJson(JsonDocument& doc);

    // Message handlers
    void HandleGetSettings(uint8_t clientId);
    void HandleSetSetting(uint8_t clientId, JsonDocument& doc);
    void HandleStart(uint8_t clientId);
    void HandleStop(uint8_t clientId);
    void HandleRestart(uint8_t clientId);
    void HandleFactoryReset(uint8_t clientId);
    void HandleAuth(uint8_t clientId, JsonDocument& doc);
    void HandleIsAuth(uint8_t clientId);
    void HandlePing(uint8_t clientId);
};

#endif