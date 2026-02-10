#include "SNTP.h"
#include <WiFi.h>

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;

void SNTP::Init() 
{
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

SNTP::~SNTP() 
{

}

struct tm SNTP::GetLocalTime()
{
  struct tm timeInfo;
  getLocalTime(&timeInfo);
  
  return timeInfo;
}