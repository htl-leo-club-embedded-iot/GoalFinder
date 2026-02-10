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
        Serial.println("Update started");
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
            Serial.println("Update complete");
        }
    }
}

bool SoftwareUpdater::IsUpdating() {
    return Update.isRunning();
}