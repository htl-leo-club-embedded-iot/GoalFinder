#pragma once

#include <Adafruit_VL53L0X.h>

class ToFSensor 
{
    public: 
        virtual ~ToFSensor();
        
        ToFSensor() : wireConfig(0) 
        {
            
        }

        void Init(int sclPin, int sdaPin);
        int ReadSingleMillimeters();
    private: 
        Adafruit_VL53L0X sensor; 
        TwoWire wireConfig;
};