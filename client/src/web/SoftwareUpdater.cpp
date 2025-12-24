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
        ESP.restart();
    }, HandleUpdate);
}

void SoftwareUpdater::HandleUpdate(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    if (!index)
    {
        //update_status = "Update started!";
        Serial.println("Update started");
        int update_content_len = request->contentLength();

        if (!Update.begin(update_content_len, U_FLASH)) //, U_FLASH
        {
            Update.printError(Serial);
        }
    }

    if (Update.write(data, len) != len)
    {
        Update.printError(Serial);
    }
    else
    {
        //update_status = "Progress: " + String((Update.progress() * 100) / Update.size()) + "%";
        //Serial.println(update_status);
    }

    if (final)
    {
        AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Please wait while the device reboots...");
        response->addHeader("Refresh", "5");
        response->addHeader("Location", "/");
        request->send(response);
        
        if (!Update.end(true))
        {
            Update.printError(Serial);
        }
        else
        {
            //update_status = "Done!";
            //Serial.println("Update complete");
            //Serial.flush();
            //eDM_Config.Flags.RebootESP = 1;
        }
    }
}

bool SoftwareUpdater::IsUpdating() {
    return Update.isRunning();
}