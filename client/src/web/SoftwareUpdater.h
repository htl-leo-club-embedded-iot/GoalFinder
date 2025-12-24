#pragma once
#include <ESPAsyncWebServer.h>

class SoftwareUpdater 
{
    public:
        SoftwareUpdater(AsyncWebServer* server);
        void Begin(const char* uri);
        static bool IsUpdating();
    private:
        AsyncWebServer* server;
        static void HandleUpdate(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
};