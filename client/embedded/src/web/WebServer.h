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

#ifndef ___WEB_SERVER_H
#define ___WEB_SERVER_H

#include <ESPAsyncWebServer.h>
#include <FileSystem.h>
// forward declare to break include cycle
class GoalfinderApp;
#include <AsyncJson.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include "SoftwareUpdater.h"
#include "Settings.h"
#include "version.h"
#include "util/Logger.h"

class WebServer 
{
    public:
        virtual ~WebServer();
        void Begin();
        void Stop();
        WebServer(FileSystem* fileSystem);
    private:
        AsyncWebServer server;
        SoftwareUpdater updater;
        void Init();
        bool isDone;
};

#endif