#pragma once
#include <Arduino.h>
#include <WiFi.h>

class WifiManager 
{
    public:
        virtual ~WifiManager();
        void Connect();
        void CreateAccessPoint(String ssid, String password);
        void Disconnect();
};