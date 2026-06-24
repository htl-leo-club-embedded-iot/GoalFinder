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
#include <ctype.h>
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
        String result = uri;

        if (q >= 0 && h >= 0) {
            cut = q < h ? q : h;
        } else if (q >= 0) {
            cut = q;
        } else if (h >= 0) {
            cut = h;
        }

        if (cut >= 0) {
            result = uri.substring(0, cut);
        }

        return result;
    }

    String StripPortFromHost(const String& host) {
        // Keep IPv6 literals (multiple ':') untouched; only strip host:port patterns.
        int firstColon = host.indexOf(':');
        int lastColon = host.lastIndexOf(':');
        String strippedHost = host;

        if (firstColon > 0 && firstColon == lastColon) {
            strippedHost = host.substring(0, firstColon);
        }

        return strippedHost;
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

        String normalizedHost = host;
        return normalizedHost;
    }

    String BuildAliasHostFromDeviceName(String deviceName) {
        String label;
        String aliasHost;
        label.reserve(deviceName.length());
        bool lastWasSeparator = false;

        for (size_t i = 0; i < deviceName.length(); ++i) {
            unsigned char c = static_cast<unsigned char>(deviceName.charAt(i));

            if (isalnum(c)) {
                label += static_cast<char>(tolower(c));
                lastWasSeparator = false;
                continue;
            }

            bool isSeparator = c == ' ' || c == '-' || c == '_' || c == '.';
            if (isSeparator && !lastWasSeparator && !label.isEmpty()) {
                label += '-';
                lastWasSeparator = true;
            }
        }

        while (label.endsWith("-")) {
            label.remove(label.length() - 1);
        }

        if (!label.isEmpty()) {
            aliasHost = label + ".local";
        }

        return aliasHost;
    }

    bool IsHostMatch(const String& normalizedHost, const String& expectedHost) {
        bool isMatch = false;

        if (!expectedHost.isEmpty()) {
            if (normalizedHost == expectedHost) {
                isMatch = true;
            } else {
                String hostSuffix = "." + expectedHost;
                isMatch = normalizedHost.endsWith(hostSuffix);
            }
        }

        return isMatch;
    }

    bool IsAllowedPortalHost(const String& normalizedHost, const String& apIp, const String& deviceAliasHost) {
        bool isAllowed = normalizedHost.isEmpty() ||
                         normalizedHost == apIp ||
                         IsHostMatch(normalizedHost, "goalfinder.local") ||
                         IsHostMatch(normalizedHost, deviceAliasHost);

        return isAllowed;
    }

    String ResolvePortalRedirectHost(const String& normalizedHost, const String& apIp, const String& deviceAliasHost) {
        String redirectHost = "goalfinder.local";

        if (!normalizedHost.isEmpty() && IsAllowedPortalHost(normalizedHost, apIp, deviceAliasHost)) {
            redirectHost = normalizedHost;
        }

        return redirectHost;
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
    return uri.lastIndexOf('.') > uri.lastIndexOf('/') && uri.lastIndexOf('.') > 0;
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

    auto buildRedirectUrl = [this](String& urlOut) {
        String host = NormalizeHostForPortal(server.hostHeader());
        String apIp = WiFi.softAPIP().toString();
        String deviceAliasHost = NormalizeHostForPortal(BuildAliasHostFromDeviceName(Settings::GetInstance()->GetDeviceName()));
        String redirectHost = ResolvePortalRedirectHost(host, apIp, deviceAliasHost);
        urlOut = "http://" + redirectHost;
    };

    auto redirectHandler = [this, buildRedirectUrl]() {
        String url;
        buildRedirectUrl(url);
        server.sendHeader("Location", url, true);
        server.send(302, "text/plain", "");
    };

    auto portalHtmlHandler = [this, buildRedirectUrl]() {
        String url;
        buildRedirectUrl(url);
        String escapedUrl = url;
        escapedUrl.replace("'", "\\'");
        String html = "<!DOCTYPE html><html><head>"
                      "<meta http-equiv='refresh' content='0; url=" + url + "'>"
                      "<script>window.location.replace('" + escapedUrl + "');</script>"
                      "</head><body>"
                      "<p>Redirecting to <a href='" + url + "'>GoalFinder</a>...</p>"
                      "</body></html>";
        server.send(200, "text/html", html);
    };

    server.on("/generate_204", HTTP_GET, redirectHandler);
    server.on("/gen_204", HTTP_GET, redirectHandler);
    server.on("/204", HTTP_GET, redirectHandler);
    server.on("/mobile/status.php", HTTP_GET, redirectHandler);
    server.on("/hotspot-detect.html", HTTP_GET, redirectHandler);
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
        String deviceAliasHost = NormalizeHostForPortal(BuildAliasHostFromDeviceName(Settings::GetInstance()->GetDeviceName()));
        if ((WiFi.getMode() & WIFI_AP) && !IsAllowedPortalHost(host, apIp, deviceAliasHost)) {
            String url = "http://" + ResolvePortalRedirectHost(host, apIp, deviceAliasHost);
            server.sendHeader("Location", url, true);
            server.send(302, "text/plain", "");
            handled = true;
        }

        // Handle CORS preflight
        if (!handled && server.method() == HTTP_OPTIONS) {
            server.sendHeader("Access-Control-Allow-Origin", "*");
            server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
            server.sendHeader("Access-Control-Allow-Headers", "Content-Type");            
            server.sendHeader("Cache-Control", "max-age=604800");
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
    Logger::Log("HttpServer", Logger::LogLevel::OK, "HTTP server started");
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
