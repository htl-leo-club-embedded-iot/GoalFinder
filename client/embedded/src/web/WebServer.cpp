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

#include "WebServer.h"
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <GoalfinderApp.h>

#include "Settings.h"
#include "version.h"

#define WEBAPP_DIR "/web"
#define INDEX_PATH "/index.html"
#define COMPRESSED_FILE_EXTENSION ".gz"

#define API_URL "/api"
#define MAX_AUTH_ATTEMPTS 5
#define AUTH_TIMEOUT_MS 60000 // 1 minute

FileSystem* internalFS;

// Rate limiting for auth endpoint
static unsigned long authAttempts[MAX_AUTH_ATTEMPTS];
static int authAttemptCount = 0;
static bool authTimedOut = false;
static unsigned long authTimeoutStart = 0;

static String GetContentType(const String* fileName) 
{
    if(fileName == 0) 
    {
        return "";
    }
    else if(fileName->endsWith("html")) 
    {
        return "text/html";
    }
    else if(fileName->endsWith("css")) 
    {
        return "text/css";
    }
    else if(fileName->endsWith("js")) 
    {
        return "application/javascript";
    }
    else if(fileName->endsWith("ico")) 
    {
        return "image/x-icon";
    }
    else if(fileName->endsWith("png")) 
    {
        return "image/png";
    }
    else if(fileName->endsWith("svg")) 
    {
        return "image/svg+xml";
    }
    else if(fileName->endsWith("jpg") || fileName->endsWith("jpeg")) 
    {
        return "image/jpeg";
    }
    else if(fileName->endsWith("json"))
    {
        return "application/json";
    }
    else if(fileName->endsWith("woff2"))
    {
        return "font/woff2";
    }
    else if(fileName->endsWith("woff"))
    {
        return "font/woff";
    }
    else if(fileName->endsWith("ttf"))
    {
        return "font/ttf";
    }
    else if(fileName->endsWith("txt"))
    {
        return "text/plain";
    }
    else 
    {
        Serial.printf("[WARN][WebServer.cpp] Unknown file type for: %s\n", fileName->c_str());
        return "application/octet-stream";
    }
}

static void HandleNotFound(AsyncWebServerRequest* request) 
{
    // Captive portal: redirect any unknown host to the AP
    if (request->host() != WiFi.softAPIP().toString()) 
    {
        request->redirect("http://" + WiFi.softAPIP().toString() + "/games");
        return;
    }
    Serial.println("[WARN][WebServer.cpp] Failed request: " + request->url());
    request->send(404, "text/plain", "Not found");
}

static void HandleRequest(AsyncWebServerRequest* request)
{
    Serial.printf("[INFO][WebServer.cpp] Received request %s\n", request->url().c_str());

    // Captive portal: redirect requests coming from non-AP hosts (e.g. connectivity checks)
    // so they don't get served SPA content instead of a proper portal response
    if (request->host() != WiFi.softAPIP().toString() && request->host() != "" ) 
    {
        request->redirect("http://" + WiFi.softAPIP().toString() + "/games");
        return;
    }

    String filePath = WEBAPP_DIR + request->url();  
    String contentType = GetContentType(&filePath);

    // Try the gzip-compressed version first
    String compressedPath = filePath + COMPRESSED_FILE_EXTENSION;
    bool isCompressed = internalFS->FileExists(compressedPath);
    bool fileExists = isCompressed;

    // If no compressed version, try the uncompressed file directly
    if(!fileExists)
    {
        fileExists = internalFS->FileExists(filePath);
    }

    // If neither exists, fall back to index.html (SPA routing)
    if(!fileExists)
    {
        compressedPath = WEBAPP_DIR INDEX_PATH COMPRESSED_FILE_EXTENSION;
        isCompressed = internalFS->FileExists(compressedPath);
        fileExists = isCompressed;
        contentType = "text/html";
    }

    if(!fileExists)
    {
        Serial.printf("[WARN][WebServer.cpp] File not found: %s\n", filePath.c_str());
        request->send(404, "text/plain", "File not found");
        return;
    }

    String servePath = isCompressed ? compressedPath : filePath;
    AsyncWebServerResponse* response = request->beginResponse(LittleFS, servePath, contentType);

    if(response == nullptr)
    {
        Serial.println("[ERROR][WebServer.cpp] Failed to create response");
        request->send(500, "text/plain", "Internal server error");
        return;
    }
   
    if(isCompressed) {
        response->addHeader("Content-Encoding", "gzip");
    }

    // Don't cache index.html so the browser always fetches fresh asset references
    if(filePath.endsWith("index.html" COMPRESSED_FILE_EXTENSION)) {
        response->addHeader("Cache-Control", "no-cache");
    } else {
        response->addHeader("Cache-Control", "max-age=604800"); // 1 week
    }



    //File currFile = internalFS->OpenFile(filePath);  

    /*AsyncWebServerResponse *response = request->beginChunkedResponse(contentType, [currFile](uint8_t *buffer, size_t maxLen, size_t index) -> size_t 
    {
        File file = currFile;
        return file.read(buffer, maxLen);
    });

    request->onDisconnect([currFile] 
    {
        File file = currFile;
        file.close();
    }); */

    //AsyncWebServerResponse* response = request->beginResponse(LittleFS, filePath, contentType);
    //response->addHeader("Cache-Control", "max-age=604800");
    //response->addHeader("Transfer-Encoding", "chunked");
    //response->addHeader("Content-Encoding", "gzip"); //The complete webapp is compressed with gzip to save space and load the files faster
    request->send(response);
    //request->beginResponseStream(contentType);
}

static void HandleLoadSettings(AsyncWebServerRequest* request) 
{
    AsyncJsonResponse* response = new AsyncJsonResponse();
    response->addHeader("Server", "Settings");
    JsonVariant& root = response->getRoot();

    Settings* settings = Settings::GetInstance();    
    
    root["deviceName"] = settings->GetDeviceName();
    root["wifiPassword"] = settings->GetWifiPassword();
    root["devicePassword"] = settings->GetDevicePassword();
    root["vibrationSensorSensitivity"] = settings->GetVibrationSensorSensitivity();
    root["ballHitDetectionDistance"] = settings->GetBallHitDetectionDistance();
    root["distanceOnlyHitDetection"] = settings->GetDistanceOnlyHitDetection();
    root["volume"] = settings->GetVolume();
    root["metronomeSound"] = settings->GetMetronomeSound();
    root["hitSound"] = settings->GetHitSound();
    root["missSound"] = settings->GetMissSound();
    root["ledMode"] = (int)settings->GetLedMode();
    root["ledBrightness"] = settings->GetLedBrightness();
    root["macAddress"] = settings->GetMacAddress();
    root["isSoundEnabled"] = GoalfinderApp::GetInstance()->IsSoundEnabled();
    root["version"] = FIRMWARE_VERSION;
    root["afterHitTimeout"] = settings->GetAfterHitTimeout();

    response->setLength();
    request->send(response);
}

static void HandleSaveSettings(AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) 
{
    JsonDocument doc;

    // Null-terminate data for safe string operations
    char* jsonStr = new char[len + 1];
    memcpy(jsonStr, data, len);
    jsonStr[len] = '\0';

    Serial.printf("[INFO][WebServer.cpp] Received settings: %s\n", jsonStr);
    deserializeJson(doc, jsonStr, len);
    delete[] jsonStr;

    GoalfinderApp* app = GoalfinderApp::GetInstance();
    //app->SetIsSoundEnabled(doc["isSoundEnabled"]);

    Settings* settings = Settings::GetInstance();
    settings->SetDeviceName(doc["deviceName"]);
    if (!doc["wifiPassword"].isNull()) {
        settings->SetWifiPassword(doc["wifiPassword"]);
    }
    settings->SetDevicePassword(doc["devicePassword"]);
    settings->SetVibrationSensorSensitivity(doc["vibrationSensorSensitivity"]);
    settings->SetBallHitDetectionDistance(doc["ballHitDetectionDistance"]);
    settings->SetDistanceOnlyHitDetection(doc["distanceOnlyHitDetection"]);
    settings->SetVolume(doc["volume"]);
    settings->SetMetronomeSound(doc["metronomeSound"]);
    settings->SetHitSound(doc["hitSound"]);
    settings->SetMissSound(doc["missSound"]);
    settings->SetLedMode(doc["ledMode"]);
    settings->SetAfterHitTimeout(doc["afterHitTimeout"]);
    if (!doc["ledBrightness"].isNull()) {
        settings->SetLedBrightness(doc["ledBrightness"]);
    }

    request->send(204);
}

static void HandleHits(AsyncWebServerRequest* request) {
    int hits = GoalfinderApp::GetInstance()->GetDetectedHits();

    GoalfinderApp::GetInstance()->ResetDetectedHits();

    request->send(200, "text/plain", String(hits));
}

static void HandleMisses(AsyncWebServerRequest* request) {
    int misses = GoalfinderApp::GetInstance()->GetDetectedMisses();

    GoalfinderApp::GetInstance()->ResetDetectedMisses();

    request->send(200, "text/plain", String(misses));
}

static void HandleRestart(AsyncWebServerRequest* request) 
{
    request->send(204);
    delay(200);
    ESP.restart();
}

static void HandleFactoryReset(AsyncWebServerRequest* request) 
{
    request->send(204);
    Settings::GetInstance()->ResetToDefaults();
}

static void HandleStart(AsyncWebServerRequest* request) {
    GoalfinderApp::GetInstance()->SetIsSoundEnabled(true);
    request->send(204);
}

static void HandleStop(AsyncWebServerRequest* request) {
    GoalfinderApp::GetInstance()->SetIsSoundEnabled(false);
    request->send(204);
}

static void HandleConnection(AsyncWebServerRequest* request) {
    AsyncJsonResponse* response = new AsyncJsonResponse();
    response->addHeader("Server", "GoalFinder");
    JsonVariant& root = response->getRoot();
    
    root["success"] = true;
    root["message"] = "Connected to device";
    
    response->setLength();
    request->send(response);
}

static void HandleUpdateStatus(AsyncWebServerRequest* request) {
    AsyncJsonResponse* response = new AsyncJsonResponse();
    response->addHeader("Server", "GoalFinder");
    JsonVariant& root = response->getRoot();

    Settings* settings = Settings::GetInstance();
    bool updateSuccess = settings->GetUpdateSuccess();

    root["updateSuccess"] = updateSuccess;

    if (updateSuccess) {
        settings->SetUpdateSuccess(false);
    }

    response->setLength();
    request->send(response);
}

static void HandleAuth(AsyncWebServerRequest* request) {
    AsyncJsonResponse* response = new AsyncJsonResponse();
    response->addHeader("Server", "GoalFinder");
    JsonVariant& root = response->getRoot();
    
    unsigned long now = millis();
    
    // Check if we're in timeout state
    if (authTimedOut) {
        if (now - authTimeoutStart < AUTH_TIMEOUT_MS) {
            root["success"] = false;
            root["error"] = "Too many attempts. Please wait.";
            root["timeout"] = true;
            response->setLength();
            request->send(response);
            return;
        } else {
            // Timeout expired, reset
            authTimedOut = false;
            authAttemptCount = 0;
        }
    }
    
    // Clean old attempts (older than 1 minute)
    int validAttempts = 0;
    for (int i = 0; i < authAttemptCount; i++) {
        if (now - authAttempts[i] < AUTH_TIMEOUT_MS) {
            authAttempts[validAttempts++] = authAttempts[i];
        }
    }
    authAttemptCount = validAttempts;
    
    // Check rate limit
    if (authAttemptCount >= MAX_AUTH_ATTEMPTS) {
        authTimedOut = true;
        authTimeoutStart = now;
        root["success"] = false;
        root["error"] = "Too many attempts. Please wait.";
        root["timeout"] = true;
        response->setLength();
        request->send(response);
        return;
    }
    
    // Record this attempt
    authAttempts[authAttemptCount++] = now;
    
    // Check if password parameter exists
    if (!request->hasParam("password")) {
        root["success"] = false;
        root["error"] = "Password parameter required";
        response->setLength();
        request->send(response);
        return;
    }
    
    // Validate password
    String providedPassword = request->getParam("password")->value();
    String correctPassword = Settings::GetInstance()->GetDevicePassword();
    
    if (providedPassword == correctPassword) {
        root["success"] = true;
        root["message"] = "Authentication successful";
    } else {
        root["success"] = false;
        root["error"] = "Invalid password";
    }
    
    response->setLength();
    request->send(response);
}

static void HandleIsAuth(AsyncWebServerRequest* request) {
    AsyncJsonResponse* response = new AsyncJsonResponse();
    response->addHeader("Server", "GoalFinder");
    JsonVariant& root = response->getRoot();
    
    String devicePassword = Settings::GetInstance()->GetDevicePassword();
    bool hasPassword = !devicePassword.isEmpty();
    
    root["isPasswordProtected"] = hasPassword;
    
    response->setLength();
    request->send(response);
}

WebServer::WebServer(FileSystem* fileSystem) : server(80), updater(&server)
{
    internalFS = fileSystem;
    Init();
}

void WebServer::Init() 
{
    server.rewrite("", INDEX_PATH);
    server.rewrite("/", INDEX_PATH);
    server.onNotFound(HandleNotFound); 
    
    // TODO Replace any serial.out with "Log"
    Serial.println("[OK][WebServer.cpp] Web server initialized");
}

void WebServer::Begin() 
{
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");

    updater.Begin(API_URL"/update");

    // Captive portal detection endpoints
    // Return 200 + HTML (not 204, not 302) so all platforms reliably detect the portal.
    // Android probes /generate_204 and /gen_204 — anything other than 204 triggers the sign-in sheet.
    // iOS looks for specific body content in /hotspot-detect.html.
    // Windows & Firefox each have their own probe URLs.
    auto captiveHandler = [](AsyncWebServerRequest *request) {
        String portalUrl = "http://" + WiFi.softAPIP().toString() + "/games";
        String html = "<!DOCTYPE html><html><head>"
                      "<meta http-equiv='refresh' content='0; url=" + portalUrl + "'>"
                      "</head><body>"
                      "<p>Redirecting to <a href='" + portalUrl + "'>GoalFinder</a>...</p>"
                      "</body></html>";
        request->send(200, "text/html", html);
    };
    // Android
    server.on("/generate_204", HTTP_GET, captiveHandler);
    server.on("/gen_204", HTTP_GET, captiveHandler);
    server.on("/204", HTTP_GET, captiveHandler);
    // iOS / macOS
    server.on("/hotspot-detect.html", HTTP_GET, captiveHandler);
    // Windows
    server.on("/ncsi.txt", HTTP_GET, captiveHandler);
    server.on("/connecttest.txt", HTTP_GET, captiveHandler);
    server.on("/fwlink", HTTP_GET, captiveHandler);
    // Firefox
    server.on("/canonical.html", HTTP_GET, captiveHandler);
    server.on("/success.txt", HTTP_GET, captiveHandler);
    // Generic fallback
    server.on("/redirect", HTTP_GET, captiveHandler);

    server.on(API_URL"/start", HTTP_POST, HandleStart);
    server.on(API_URL"/stop", HTTP_POST, HandleStop);
    server.on(API_URL"/connection", HTTP_GET, HandleConnection);
    server.on(API_URL"/update-status", HTTP_GET, HandleUpdateStatus);
    server.on(API_URL"/auth", HTTP_GET, HandleAuth);
    server.on(API_URL"/isauth", HTTP_GET, HandleIsAuth);
    server.on(API_URL"/settings", HTTP_GET, HandleLoadSettings);
    server.on(API_URL"/settings", HTTP_POST, [](AsyncWebServerRequest* request) {}, 0, HandleSaveSettings);
    server.on(API_URL"/restart", HTTP_POST, HandleRestart);    
    server.on(API_URL"/factory-reset", HTTP_POST, HandleFactoryReset);
    server.on(API_URL"/hits", HTTP_GET, HandleHits);
    server.on(API_URL"/misses", HTTP_GET, HandleMisses);
    server.on("/*", HTTP_GET, HandleRequest);

    server.onNotFound([](AsyncWebServerRequest *request) {
        if (request->method() == HTTP_OPTIONS) {
            // Handle CORS preflight
            AsyncWebServerResponse* response = request->beginResponse(200);
            response->addHeader("Access-Control-Allow-Origin", "*");
            response->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
            response->addHeader("Access-Control-Allow-Headers", "Content-Type");
            request->send(response);
        } else if (request->host() != WiFi.softAPIP().toString()) {
            // Captive portal: any request to a non-AP host gets redirected
            request->redirect("http://" + WiFi.softAPIP().toString() + "/games");
        } else {
            request->send(404, "text/plain", "Not found");
        }
    });

    //server.serveStatic("/", LittleFS, "/web/").setDefaultFile("index.html").setCacheControl("max-age=604800");
    server.begin();
    Serial.println("[OK][WebServer.cpp] Started web server");
}

void WebServer::Stop() 
{    
    server.end();
}

WebServer::~WebServer()
{
}