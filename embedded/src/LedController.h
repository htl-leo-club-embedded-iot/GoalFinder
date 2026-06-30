/*
 * ===============================================================================
 * (c) HTBLA Leonding 2024 - 2026
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Licensed under MIT License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the license.
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * All trademarks used in this document are property of their respective owners.
 * ===============================================================================
 */

#pragma once
#include "LedMode.h"
#include "Settings.h"
#include <stdint.h>


class LedController
{
    private:    
        void RenderPermanentStep(uint16_t brightness);
        void RenderFadeStep();
        void RenderFlashStep();
        void RenderTurboStep();
        void RenderHitFlash();
        void RenderMissFade();
        void RenderShotBreathing();
        void ClearEvent();
        uint16_t ScaleBrightness(uint16_t value);

        int channel;
        LedMode mode;
        unsigned long lastStepTimeMs;
        uint16_t lastAppliedDuty;

        bool _eventActive;
        EventEffect _eventType;
        LedMode _savedMode;
        uint8_t _eventPhase;
        unsigned long _eventStartTime;
        uint16_t _eventBaseDuty;

        uint8_t _eventFlashCount;
        unsigned long _eventFlashOnMs;
        unsigned long _eventFlashOffMs;
        unsigned long _eventFadeDurationMs;
        unsigned long _eventBreatheCycleMs;
        float _eventBreatheMaxFraction;

    public:
        LedController(int ledPin, int ledChannel);
        ~LedController();
        void Loop();
        void SetMode(LedMode mode);
        LedMode GetMode();
        void OnHit();
        void OnMiss();
        void OnShotDetected();
};