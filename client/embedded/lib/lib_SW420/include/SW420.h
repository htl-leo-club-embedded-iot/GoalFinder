#pragma once

#include <stdint.h>

class SW420 {
    public: 
        virtual ~SW420();
        void Init();
        long Vibration(uint64_t measureTimeUs);
        void SetSensitivity(int sensitivity);
        
    private:
        int vs;
};

