#pragma once
#include "LedMode.h"
#include <stdint.h>


class LedController
{
    private:    
        void RenderPermanentStep(uint8_t brightness);
        void RenderFadeStep();
        void RenderFlashStep();
        void RenderTurboStep();

        int channel;
        LedMode mode;
        uint64_t lastStepTimeMs;

    public:
        LedController(int ledPin, int ledChannel);
        ~LedController();
        void Loop();
        void SetMode(LedMode mode);
        LedMode GetMode();
};