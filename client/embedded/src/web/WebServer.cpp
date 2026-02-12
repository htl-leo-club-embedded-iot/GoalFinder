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

FileSystem* internalFS;

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
    else 
    {
        //Serial.println("[ERROR] Unknown file type to get content type."); // Debugging: Handle unknown file types
        return "";
    }
}

static void HandleNotFound(AsyncWebServerRequest* request) 
{
    // Captive portal: redirect any unknown host to the AP
    if (request->host() != WiFi.softAPIP().toString()) 
    {
        request->redirect("http://" + WiFi.softAPIP().toString() + "/");
        return;
    }
    Serial.println("Failed Request: " + request->url());
    request->send(404, "text/plain", "Not found");
}

static void HandleRequest(AsyncWebServerRequest* request)
{
    Serial.printf("Web Server: received request %s\n", request->url().c_str());

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
        Serial.printf("Web Server: file not found: %s\n", filePath.c_str());
        request->send(404, "text/plain", "File not found");
        return;
    }

    String servePath = isCompressed ? compressedPath : filePath;
    AsyncWebServerResponse* response = request->beginResponse(LittleFS, servePath, contentType);

    if(response == nullptr)
    {
        Serial.println("Web Server: failed to create response");
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
    root["devicePassword"] = settings->GetDevicePassword();
    root["vibrationSensorSensitivity"] = settings->GetVibrationSensorSensitivity();
    root["ballHitDetectionDistance"] = settings->GetBallHitDetectionDistance();
    root["volume"] = settings->GetVolume();
    root["metronomeSound"] = settings->GetMetronomeSound();
    root["hitSound"] = settings->GetHitSound();
    root["missSound"] = settings->GetMissSound();
    root["ledMode"] = (int)settings->GetLedMode();
    root["macAddress"] = settings->GetMacAddress();
    root["isSoundEnabled"] = GoalfinderApp::GetInstance()->IsSoundEnabled();
    root["version"] = FIRMWARE_VERSION;

    response->setLength();
    request->send(response);
}

static void HandleSaveSettings(AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) 
{
    JsonDocument doc;
        
    Serial.printf("Web Server: received settings: %s\n", data);
    deserializeJson(doc, (const char*)data);

    GoalfinderApp* app = GoalfinderApp::GetInstance();
    //app->SetIsSoundEnabled(doc["isSoundEnabled"]);

    Settings* settings = Settings::GetInstance();
    settings->SetDeviceName(doc["deviceName"]);
    settings->SetDevicePassword(doc["devicePassword"]);
    settings->SetVibrationSensorSensitivity(doc["vibrationSensorSensitivity"]);
    settings->SetBallHitDetectionDistance(doc["ballHitDetectionDistance"]);
    settings->SetVolume(doc["volume"]);
    settings->SetMetronomeSound(doc["metronomeSound"]);
    settings->SetHitSound(doc["hitSound"]);
    settings->SetMissSound(doc["missSound"]);
    settings->SetLedMode(doc["ledMode"]);

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
    ESP.restart();
    request->send(204);
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
    Serial.println("Web server initialized!");
}

void WebServer::Begin() 
{
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");

    updater.Begin(API_URL"/update");

    // === Captive portal detection endpoints ===
    // Android
    server.on("/generate_204", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->redirect("http://" + WiFi.softAPIP().toString() + "/");
    });
    server.on("/gen_204", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->redirect("http://" + WiFi.softAPIP().toString() + "/");
    });
    // iOS / macOS
    server.on("/hotspot-detect.html", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->redirect("http://" + WiFi.softAPIP().toString() + "/");
    });
    // Windows
    server.on("/ncsi.txt", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->redirect("http://" + WiFi.softAPIP().toString() + "/");
    });
    server.on("/connecttest.txt", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->redirect("http://" + WiFi.softAPIP().toString() + "/");
    });
    // Firefox
    server.on("/canonical.html", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->redirect("http://" + WiFi.softAPIP().toString() + "/");
    });
    // Generic fallback
    server.on("/redirect", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->redirect("http://" + WiFi.softAPIP().toString() + "/");
    });

    server.on(API_URL"/start", HTTP_POST, HandleStart);
    server.on(API_URL"/stop", HTTP_POST, HandleStop);
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
        } else {
            request->send(404);
        }
    });

    //server.serveStatic("/", LittleFS, "/web/").setDefaultFile("index.html").setCacheControl("max-age=604800");
    server.begin();
    Serial.println("[INFO] Started web server.");
}

void WebServer::Stop() 
{    
    server.end();
}

WebServer::~WebServer()
{
}