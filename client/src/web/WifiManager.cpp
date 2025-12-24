#include "WifiManager.h"

void WifiManager::Connect() 
{
    WiFi.mode(WIFI_STA);
    WiFi.begin();
}

void WifiManager::CreateAccessPoint(String ssid, String password) 
{
    // Remove the password parameter, if you want the AP (Access Point) to be open
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);
}

void WifiManager::Disconnect() 
{
    WiFi.disconnect(false, true);
}

WifiManager::~WifiManager()
{
    
}