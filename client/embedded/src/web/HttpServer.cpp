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
#include <uri/UriGlob.h>
#include "Settings.h"

#define WEBAPP_DIR "/web"
#define INDEX_PATH "/index.html"
#define COMPRESSED_FILE_EXTENSION ".gz"
#define API_URL "/api"

static HttpServer* _instance = nullptr;
volatile bool g_httpServingFile = false;

namespace {
    String StripQueryAndFragment(const String& uri) {
        int q = uri.indexOf('?');
        int h = uri.indexOf('#');
        int cut = -1;

        if (q >= 0 && h >= 0) {
            cut = q < h ? q : h;
        } else if (q >= 0) {
            cut = q;
        } else if (h >= 0) {
            cut = h;
        }

        return cut < 0 ? uri : uri.substring(0, cut);
    }

    String StripPortFromHost(const String& host) {
        return host.indexOf(':') > 0 && host.indexOf(':') == host.lastIndexOf(':') ? host.substring(0, host.indexOf(':')) : host;
    }

    bool ShouldTryCompressedVariant(const String& filePath) {
        String lower = filePath;
        lower.toLowerCase();

        return lower.endsWith(".html") ||
               lower.endsWith(".css")  ||
               lower.endsWith(".js")   ||
               lower.endsWith(".json") ||
               lower.endsWith(".svg")  ||
               lower.endsWith(".txt")  ||
               lower.endsWith(".map");
    }

    String NormalizeHostForPortal(const String& hostHeader) {
        String host = StripPortFromHost(hostHeader);
        host.trim();
        host.toLowerCase();

        while (host.endsWith(".")) {
            host.remove(host.length() - 1);
        }

        return host;
    }

    bool IsAllowedPortalHost(const String& normalizedHost, const String& apIp) {
        return normalizedHost.isEmpty() || normalizedHost == apIp || normalizedHost == "goalfinder.local" || normalizedHost.endsWith(".goalfinder.local");
    }
}

String HttpServer::GetContentType(const String& fileName) {
    if (fileName.endsWith("html"))      return "text/html";
    if (fileName.endsWith("css"))       return "text/css";
    if (fileName.endsWith("js"))        return "application/javascript";
    if (fileName.endsWith("ico"))       return "image/x-icon";
    if (fileName.endsWith("png"))       return "image/png";
    if (fileName.endsWith("svg"))       return "image/svg+xml";
    if (fileName.endsWith("jpg"))       return "image/jpeg";
    if (fileName.endsWith("jpeg"))       return "image/jpeg";
    if (fileName.endsWith("json"))      return "application/json";
    if (fileName.endsWith("woff2"))     return "font/woff2";
    if (fileName.endsWith("woff"))      return "font/woff";
    if (fileName.endsWith("ttf"))       return "font/ttf";
    if (fileName.endsWith("txt"))       return "text/plain";
    if (fileName.endsWith("gz"))        return "application/gzip";
    return "";
}

bool HttpServer::HasFileExtension(const String& uri) {
    return lastDot > uri.lastIndexOf('/') && uri.lastIndexOf('.') > 0;
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

void HttpServer::Init() {
    Logger::Log("HttpServer", Logger::LogLevel::OK, "HTTP server initialized");
}

void HttpServer::Begin() {
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
    server.on("/hotspot-detect.html", HTTP_GET, htmlHandler);
    server.on("/ncsi.txt", HTTP_GET, redirectHandler);
    server.on("/connecttest.txt", HTTP_GET, redirectHandler);
    server.on("/fwlink", HTTP_GET, redirectHandler);
    server.on("/canonical.html", HTTP_GET, redirectHandler);
    server.on("/success.txt", HTTP_GET, redirectHandler);
    server.on("/redirect", HTTP_GET, redirectHandler);

    server.on(API_URL "/update", HTTP_OPTIONS, [this]() {
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
        server.send(200);
    });

    auto webFallbackHandler = [this]() {
        String uri = StripQueryAndFragment(server.uri());
        if (uri.isEmpty()) {
            uri = "/";
        }

        bool handled = false;

        // Captive portal: redirect requests from non-AP hosts
        String host = NormalizeHostForPortal(server.hostHeader());
        String apIp = WiFi.softAPIP().toString();
        if ((WiFi.getMode() & WIFI_AP) && !IsAllowedPortalHost(host, apIp)) {
            String url = "http://" + apIp + "/games";
            server.sendHeader("Location", url, true);
            server.send(302, "text/plain", "");
            handled = true;
        }

        // Handle CORS preflight
        if (!handled && server.method() == HTTP_OPTIONS) {
            server.sendHeader("Access-Control-Allow-Origin", "*");
            server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
            server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
            server.send(200);
            handled = true;
        }

        if (!handled) {
            bool isFileRequest = HasFileExtension(uri);

            if (isFileRequest) {
                String filePath = WEBAPP_DIR + uri;
                bool requestedGzip = filePath.endsWith(COMPRESSED_FILE_EXTENSION);
                String contentType = GetContentType(requestedGzip
                                                    ? filePath.substring(0, filePath.length() - 3)
                                                    : filePath);
                if (contentType.isEmpty()) {
                    contentType = "application/octet-stream";
                }

                if (requestedGzip) {
                    if (!fs->FileExists(filePath)) {
                        server.send(404, "text/plain", "Not found");
                    } else {
                        ServeFile(filePath, contentType, true, filePath.endsWith("index.html.gz"));
                    }
                } else {
                    // Try gzip-compressed version first
                    String compressedPath = filePath + COMPRESSED_FILE_EXTENSION;
                    bool isCompressed = ShouldTryCompressedVariant(filePath) && fs->FileExists(compressedPath);
                    bool fileExists = isCompressed || fs->FileExists(filePath);

                    if (!fileExists) {
                        server.send(404, "text/plain", "Not found");
                    } else {
                        String servePath = isCompressed ? compressedPath : filePath;
                        ServeFile(servePath, contentType, isCompressed, filePath.endsWith("index.html"));
                    }
                }
            } else {
                // SPA fallback: serve index.html for client-side routes
                String compressedPath = WEBAPP_DIR INDEX_PATH COMPRESSED_FILE_EXTENSION;
                bool isCompressed = fs->FileExists(compressedPath);

                if (!isCompressed && !fs->FileExists(WEBAPP_DIR INDEX_PATH)) {
                    server.send(404, "text/plain", "Not found");
                } else {
                    String servePath = isCompressed ? compressedPath : String(WEBAPP_DIR INDEX_PATH);
                    ServeFile(servePath, "text/html", isCompressed, true);
                }
            }
        }
    };

    server.on(UriGlob("/*"), HTTP_ANY, webFallbackHandler);
    server.onNotFound(webFallbackHandler);

    server.begin();
    Logger::Log("HttpServer", Logger::LogLevel::INFO, "HTTP server started");
}

void HttpServer::Loop()
{
    server.handleClient();
}

void HttpServer::ServeFile(const String& path, const String& contentType, bool isCompressed, bool noCache)
{
    g_httpServingFile = true;
    File file = LittleFS.open(path, "r");
    g_httpServingFile = false;
    if (!file) {
        Logger::Log("HttpServer", Logger::LogLevel::ERROR, "Failed to open file: %s", path.c_str());
        server.send(500, "text/plain", "Internal server error");
    } else {
        if (noCache) {
            server.sendHeader("Cache-Control", "no-cache");
        } else {
            server.sendHeader("Cache-Control", "max-age=604800");
        }
    
        if (isCompressed && contentType != "application/gzip") {
            server.sendHeader("Content-Encoding", "gzip");
        }
    
        server.setContentLength(file.size());
        server.send(200, contentType, "");
    
        if (server.method() != HTTP_HEAD) {        
            uint8_t buffer[1024];
            bool continueReading = true;
            while (continueReading && file.available()) {
                size_t chunkSize = g_audioPlaybackActive ? 384 : sizeof(buffer);
                
                g_httpServingFile = true;
                size_t bytesRead = file.read(buffer, chunkSize);
                g_httpServingFile = false;
                
                if (bytesRead == 0) {
                    continueReading = false;
                } else {
                    server.sendContent(reinterpret_cast<const char*>(buffer), bytesRead);
                    delay(g_audioPlaybackActive ? 1 : 0);
                    if (g_audioPlaybackActive) {
                        delay(1);
                    }
                }
            }
        }
    }
        
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
