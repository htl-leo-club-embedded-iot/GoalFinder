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

#ifndef ___HTTP_SERVER_H
#define ___HTTP_SERVER_H

#include <WebServer.h>
#include <FileSystem.h>
#include "SoftwareUpdater.h"
#include "util/Logger.h"

class HttpServer 
{
public:
    HttpServer(FileSystem* fileSystem);
    ~HttpServer();

    /** Configure and start the HTTP server on port 80 */
    void Begin();

    /** Stop the HTTP server */
    void Stop();

    /** Process pending HTTP requests - call from FreeRTOS task loop */
    void Loop();

    /** FreeRTOS task entry point */
    static void Task(void* pvParameters);

private:
    WebServer server;
    FileSystem* fs;
    SoftwareUpdater updater;

    void Init();

    /** Determine MIME type from file extension. Returns empty string for unknown types. */
    static String GetContentType(const String& fileName);

    /** Check if a URI has a file extension (e.g. .js, .css, .png) */
    static bool HasFileExtension(const String& uri);

    /** Serve a file from LittleFS with optional gzip and cache headers */
    void ServeFile(const String& path, const String& contentType, bool isCompressed, bool noCache);
};

#endif
