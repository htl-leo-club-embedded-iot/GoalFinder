#include "util/Logger.h"
#include <Arduino.h>
#include <SW420.h>

SW420::~VibrationSensor() {
}

void SW420::Init() {
    vs = 13;
    pinMode(vs, INPUT);
}

long SW420::Vibration(uint64_t measureTimeUs) {
    long measurement = pulseIn(vs, HIGH, measureTimeUs);
    return measurement;
}

void SW420::SetSensitivity(int sensitivity) {
    if (sensitivity < 0) {
        sensitivity = 0;
    } else if (sensitivity > 100) {
        sensitivity = 100;
    }
}