#pragma once

#include <stdint.h>

class VibrationSensor {
    public: 
        virtual ~VibrationSensor();
        void Init();
        long Vibration(uint64_t measureTimeUs);

        /** 
         * Sets the sensitivity of the sensor in the range of 0 to 100%.
         * The value is clipped.
         */
        void SetSensitivity(int sensitivity);
        
    private:
        int vs;
};

