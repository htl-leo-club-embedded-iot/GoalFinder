#include "WebServer.h"
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <GoalfinderApp.h>

#include "Settings.h"

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
    else 
    {
        //Serial.println("[ERROR] Unknown file type to get content type."); // Debugging: Handle unknown file types
        return "";
    }
}

static void HandleNotFound(AsyncWebServerRequest* request) 
{
    Serial.println("Failed Request: " + request->url());
    request->send(404, "text/plain", "Not found");
}

static void HandleRequest(AsyncWebServerRequest* request)
{
    Serial.printf("Web Server: received request %s\n", request->url().c_str());

    String filePath = WEBAPP_DIR + request->url();  
    const String contentType = GetContentType(&filePath);
    filePath += COMPRESSED_FILE_EXTENSION;

    bool fileExists = internalFS->FileExists(filePath);

    if(!fileExists)
    {
        filePath = WEBAPP_DIR INDEX_PATH COMPRESSED_FILE_EXTENSION;
    }

    AsyncWebServerResponse* response = request->beginResponse(LittleFS, filePath, contentType);

    /*if(fileExists) 
    {

    }*/
   
    response->addHeader("Content-Encoding", "gzip");
    response->addHeader("Cache-Control", "max-age=604800"); // 1 week



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
    settings->SetVibrationSensorSensitivity(doc["shotSensitivity"]);
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

    server.on(API_URL"/start", HTTP_POST, HandleStart);
    server.on(API_URL"/stop", HTTP_POST, HandleStop);
    server.on(API_URL"/settings", HTTP_GET, HandleLoadSettings);
    server.on(API_URL"/settings", HTTP_POST, [](AsyncWebServerRequest* request) {}, 0, HandleSaveSettings);
    server.on(API_URL"/restart", HTTP_POST, HandleRestart);    
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