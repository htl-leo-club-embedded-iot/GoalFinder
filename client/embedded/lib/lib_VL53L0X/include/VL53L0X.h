#pragma once

#include <Adafruit_VL53L0X.h>

class VL53L0X {
  public:
    virtual ~VL53L0X();
    VL53L0X() : wireConfig(0) {
    }
    void Init(int sclPin, int sdaPin);
    int ReadSingleMillimeters();

  private:
    Adafruit_VL53L0X sensor;
    TwoWire wireConfig;
};