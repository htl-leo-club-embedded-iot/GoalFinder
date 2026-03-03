#include "util/Logger.h"
#include <ToFSensor.h>

void VL53L0X::Init(int sclPin, int sdaPin) {
    wireConfig.begin(sdaPin, sclPin);

    if (!sensor.begin(41U, false, &wireConfig)) {
        Logger::log("VL53L0X", Logger::LogLevel::ERROR, "Failed to boot VL53L0X");
    }
}

int VL53L0X::ReadSingleMillimeters() {
    VL53L0X_RangingMeasurementData_t measure;
    sensor.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

    if (measure.RangeStatus != 4) {
        return measure.RangeMilliMeter;
    } else {
        return -1;
    }

    delay(100);
}

VL53L0X::~VL53L0X() {
}