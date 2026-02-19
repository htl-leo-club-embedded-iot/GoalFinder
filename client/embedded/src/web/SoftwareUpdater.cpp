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

#include "SoftwareUpdater.h"
#include <Update.h>
// #include <AsyncOTA.h>
// #include <ElegantOTA.h>

SoftwareUpdater::SoftwareUpdater(AsyncWebServer* server) {
    this->server = server;
}

void SoftwareUpdater::Begin(const char* uri) {
    server->on(uri, HTTP_POST, [](AsyncWebServerRequest *request) {
        AsyncWebServerResponse* response = request->beginResponse(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
        response->addHeader("Connection", "close");
        request->send(response);
        delay(500); // Allow response to be sent before restarting
        ESP.restart();
    }, HandleUpdate);
}

void SoftwareUpdater::HandleUpdate(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    if (!index)
    {
        Serial.println("[INFO][SoftwareUpdater.cpp] Update started");
        int update_content_len = request->contentLength();

        if (!Update.begin(update_content_len, U_FLASH))
        {
            Update.printError(Serial);
        }
    }

    if (Update.write(data, len) != len)
    {
        Update.printError(Serial);
    }

    if (final)
    {
        if (!Update.end(true))
        {
            Update.printError(Serial);
        }
        else
        {
            Serial.println("[OK][SoftwareUpdater.cpp] Update complete");
        }
    }
}

bool SoftwareUpdater::IsUpdating() {
    return Update.isRunning();
}