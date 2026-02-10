#include <VibrationSensor.h>
#include <Arduino.h>


VibrationSensor::~VibrationSensor()
{
}

void VibrationSensor::Init() 
{
    // TODO: Make pins configurable
    vs = 13;
    pinMode(vs, INPUT);
}

long VibrationSensor::Vibration(uint64_t measureTimeUs) 
{
    
   long measurement = pulseIn(vs, HIGH, measureTimeUs);
   //long measurement = pulseIn(vs, HIGH);
   return measurement;
   
}

// TODO Add a method which detects vibration (boolean result)
// based on sensitivity and measured pulse

void VibrationSensor::SetSensitivity(int sensitivity)
{
    if (sensitivity < 0) {
        sensitivity = 0;
    } else if (sensitivity > 100) {
        sensitivity = 100;
    }

}