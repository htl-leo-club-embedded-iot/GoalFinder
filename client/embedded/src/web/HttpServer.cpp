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

#include "HttpServer.h"
#include <WiFi.h>
#include <LittleFS.h>
#include "Settings.h"

#define WEBAPP_DIR "/web"
#define INDEX_PATH "/index.html"
#define COMPRESSED_FILE_EXTENSION ".gz"
#define API_URL "/api"

static HttpServer* _instance = nullptr;

String HttpServer::GetContentType(const String& fileName)
{
    if (fileName.endsWith("html"))      return "text/html";
    if (fileName.endsWith("css"))       return "text/css";
    if (fileName.endsWith("js"))        return "application/javascript";
    if (fileName.endsWith("ico"))       return "image/x-icon";
    if (fileName.endsWith("png"))       return "image/png";
    if (fileName.endsWith("svg"))       return "image/svg+xml";
    if (fileName.endsWith("jpg") || fileName.endsWith("jpeg")) return "image/jpeg";
    if (fileName.endsWith("json"))      return "application/json";
    if (fileName.endsWith("woff2"))     return "font/woff2";
    if (fileName.endsWith("woff"))      return "font/woff";
    if (fileName.endsWith("ttf"))       return "font/ttf";
    if (fileName.endsWith("txt"))       return "text/plain";
    if (fileName.endsWith("gz"))        return "application/gzip";

    return "";
}

bool HttpServer::HasFileExtension(const String& uri)
{
    int lastDot = uri.lastIndexOf('.');
    int lastSlash = uri.lastIndexOf('/');
    return lastDot > lastSlash && lastDot > 0;
}

HttpServer::HttpServer(FileSystem* fileSystem)
    : server(80),
      fs(fileSystem),
      updater(&server)
{
    _instance = this;
    Init();
}

HttpServer::~HttpServer() {}

void HttpServer::Init()
{
    Logger::log("HttpServer", Logger::LogLevel::OK, "HTTP server initialized");
}

void HttpServer::Begin()
{
    updater.Begin(API_URL "/update");

    server.on(API_URL "/update-status", HTTP_GET, [this]() {
        Settings* settings = Settings::GetInstance();
        bool updateSuccess = settings->GetUpdateSuccess();

        String json = "{\"updateSuccess\":" + String(updateSuccess ? "true" : "false") + "}";
        if (updateSuccess) {
            settings->SetUpdateSuccess(false);
        }

        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.send(200, "application/json", json);
    });

    auto redirectHandler = [this]() {
        String url = "http://" + WiFi.softAPIP().toString() + "/games";
        server.sendHeader("Location", url, true);
        server.send(302, "text/plain", "");
    };

    auto htmlHandler = [this]() {
        String url = "http://" + WiFi.softAPIP().toString() + "/games";
        String html = "<!DOCTYPE html><html><head>"
                      "<meta http-equiv='refresh' content='0; url=" + url + "'>"
                      "</head><body>"
                      "<p>Redirecting to <a href='" + url + "'>GoalFinder</a>...</p>"
                      "</body></html>";
        server.send(200, "text/html", html);
    };

    server.on("/generate_204", HTTP_GET, redirectHandler);
    server.on("/gen_204", HTTP_GET, redirectHandler);
    server.on("/204", HTTP_GET, redirectHandler);
    server.on("/mobile/status.php", HTTP_GET, redirectHandler);
    // iOS / macOS
    server.on("/hotspot-detect.html", HTTP_GET, htmlHandler);
    // Windows
    server.on("/ncsi.txt", HTTP_GET, redirectHandler);
    server.on("/connecttest.txt", HTTP_GET, redirectHandler);
    server.on("/fwlink", HTTP_GET, redirectHandler);
    // Firefox
    server.on("/canonical.html", HTTP_GET, redirectHandler);
    server.on("/success.txt", HTTP_GET, redirectHandler);
    // Generic fallback
    server.on("/redirect", HTTP_GET, redirectHandler);

    // CORS preflight handler
    server.on(API_URL "/update", HTTP_OPTIONS, [this]() {
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
        server.send(200);
    });

    // Catch-all: serve static files from LittleFS or SPA fallback
    server.onNotFound([this]() {
        String uri = server.uri();

        // Captive portal: redirect requests from non-AP hosts
        if ((WiFi.getMode() & WIFI_AP) && server.hostHeader() != WiFi.softAPIP().toString()
            && !server.hostHeader().isEmpty()) {
            String url = "http://" + WiFi.softAPIP().toString() + "/games";
            server.sendHeader("Location", url, true);
            server.send(302, "text/plain", "");
            return;
        }

        // Handle CORS preflight
        if (server.method() == HTTP_OPTIONS) {
            server.sendHeader("Access-Control-Allow-Origin", "*");
            server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
            server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
            server.send(200);
            return;
        }

        // Check if this looks like a file request (has a file extension) vs a SPA route
        bool isFileRequest = HasFileExtension(uri);

        if (isFileRequest) {
            // --- Attempt to serve static file from LittleFS ---
            String filePath = WEBAPP_DIR + uri;
            String contentType = GetContentType(filePath);
            if (contentType.isEmpty()) {
                contentType = "application/octet-stream";
            }

            // Try gzip-compressed version first
            String compressedPath = filePath + COMPRESSED_FILE_EXTENSION;
            bool isCompressed = fs->FileExists(compressedPath);
            bool fileExists = isCompressed;

            // Try uncompressed
            if (!fileExists) {
                fileExists = fs->FileExists(filePath);
            }

            if (!fileExists) {
                server.send(404, "text/plain", "Not found");
                return;
            }

            String servePath = isCompressed ? compressedPath : filePath;
            ServeFile(servePath, contentType, isCompressed, filePath.endsWith("index.html"));
        } else {
            // --- SPA fallback: serve index.html for client-side routes ---
            String compressedPath = WEBAPP_DIR INDEX_PATH COMPRESSED_FILE_EXTENSION;
            bool isCompressed = fs->FileExists(compressedPath);

            if (!isCompressed && !fs->FileExists(WEBAPP_DIR INDEX_PATH)) {
                server.send(404, "text/plain", "Not found");
                return;
            }

            String servePath = isCompressed ? compressedPath : String(WEBAPP_DIR INDEX_PATH);
            ServeFile(servePath, "text/html", isCompressed, true);
        }
    });

    server.begin();
    Logger::log("HttpServer", Logger::LogLevel::OK, "HTTP server started on port 80");
}

void HttpServer::Loop()
{
    server.handleClient();
}

void HttpServer::ServeFile(const String& path, const String& contentType, bool isCompressed, bool noCache)
{
    File file = LittleFS.open(path, "r");
    if (!file) {
        Logger::log("HttpServer", Logger::LogLevel::ERROR, "Failed to open file: %s", path.c_str());
        server.send(500, "text/plain", "Internal server error");
        return;
    }

    if (noCache) {
        server.sendHeader("Cache-Control", "no-cache");
    } else {
        server.sendHeader("Cache-Control", "max-age=604800");
    }

    if (isCompressed) {
        server.sendHeader("Content-Encoding", "gzip");
    }

    server.streamFile(file, contentType);
    file.close();
}

void HttpServer::Stop()
{
    server.stop();
}

void HttpServer::Task(void* pvParameters)
{
    HttpServer* httpServer = static_cast<HttpServer*>(pvParameters);
    while (true) {
        httpServer->Loop();
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}
