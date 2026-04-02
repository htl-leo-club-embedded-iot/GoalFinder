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

#include "LedController.h"
#include <HardwareSerial.h>
#include <esp32-hal.h>
#include <math.h>
#include "util/Logger.h"

namespace {
constexpr uint32_t defaultFrequency = 5000;
constexpr uint8_t defaultResolution = 12;
constexpr uint16_t maxPwmDuty = (1U << defaultResolution) - 1U;
constexpr unsigned long fadeCycleDurationMs = 1600;
constexpr float tau = 6.28318530718f;
}

LedController::LedController(int ledPin, int ledChannel) 
    : mode(LedMode::Standard), channel(ledChannel), lastStepTimeMs(0), lastAppliedDuty(0)
{
    ledcSetup(channel, defaultFrequency, defaultResolution);
    ledcAttachPin(ledPin, channel);
    ledcWrite(channel, 0);
}

LedController::~LedController() {}

void LedController::SetMode(LedMode mode)
{
    if (this->mode != mode) {
        this->mode = mode;
        lastStepTimeMs = 0;
        lastAppliedDuty = 0xFFFF;
        Logger::log("LedController", Logger::LogLevel::INFO, "%4.3f: LED mode set to '%d'", millis() / 1000.0, this->mode);
    }
}

LedMode LedController::GetMode()
{
    return mode;
}

void LedController::Loop() 
{
    if(mode == LedMode::Standard)
    {
        RenderPermanentStep(maxPwmDuty);
    }
    else if(mode == LedMode::Fade)
    {
        RenderFadeStep();
    }
    else if(mode == LedMode::Flash) 
    {
        RenderFlashStep();
    }
    else if (mode == LedMode::Turbo) 
    {
        RenderTurboStep();
    }
    else 
    {
        RenderPermanentStep(0);
    }
}

uint16_t LedController::ScaleBrightness(uint16_t value) {
    if (value == 0) return 0;
    int ledBrightness = Settings::GetInstance()->GetLedBrightness();
    ledBrightness = max(min(ledBrightness, 100), 0);
    return (uint16_t)roundf(value * ledBrightness / 100.0f);
}

void LedController::RenderPermanentStep(uint16_t brightness) {
    uint16_t scaled = ScaleBrightness(brightness);
    if (scaled != lastAppliedDuty) {
        lastAppliedDuty = scaled;
        ledcWrite(channel, lastAppliedDuty);
    }
}

void LedController::RenderFadeStep() {
    unsigned long now = millis();
    if (lastStepTimeMs == 0) {
        lastStepTimeMs = now;
    }

    const unsigned long elapsedMs = now - lastStepTimeMs;
    const unsigned long phaseMs = elapsedMs % fadeCycleDurationMs;
    const float phase = (tau * phaseMs) / fadeCycleDurationMs;
    const float normalized = 0.5f - 0.5f * cosf(phase);
    const uint16_t dutyCycle = (uint16_t)roundf(normalized * maxPwmDuty);
    const uint16_t scaledDuty = ScaleBrightness(dutyCycle);

    if (scaledDuty != lastAppliedDuty) {
        lastAppliedDuty = scaledDuty;
        ledcWrite(channel, lastAppliedDuty);
    }
}

void LedController::RenderFlashStep() {
    const unsigned long stepDurationsMs[] = { 500, 100 };
    const uint16_t dutyCycles[] = { 0, maxPwmDuty };
    static unsigned char phaseIdx = 0;

    unsigned long now = millis();
    if (lastStepTimeMs == 0) {
        phaseIdx = 0;
        lastStepTimeMs = now - stepDurationsMs[phaseIdx];
    }

    while (now - lastStepTimeMs >= stepDurationsMs[phaseIdx]) {
        // switch phase
        lastStepTimeMs += stepDurationsMs[phaseIdx];
        phaseIdx = (phaseIdx + 1) % 2;
        ledcWrite(channel, ScaleBrightness(dutyCycles[phaseIdx]));
    }
}

void LedController::RenderTurboStep() {
    const unsigned long stepInactiveDurationMs = 750;
    const unsigned long stepActiveDurationMs = 100;
    const uint32_t flashAmount = 10; // the number of flashes per period
    static uint32_t flashPhaseCount = 0; // counts on AND off phase
    static bool activePhase = true;

    unsigned long now = millis();
    if (lastStepTimeMs == 0) {
        // reset
        activePhase = true;
        flashPhaseCount = 0;
        lastStepTimeMs = now - stepActiveDurationMs;
    }

    Logger::logExtra("LedController", Logger::LogLevel::INFO, "%4.3f: LED turbo step %s '%lu'", millis() / 1000.0, activePhase ? "flash" : "dark", (unsigned long)flashPhaseCount);
    
    if (!activePhase && now - lastStepTimeMs >= stepInactiveDurationMs) {
        activePhase = true;
        flashPhaseCount = 0;
        lastStepTimeMs += (stepInactiveDurationMs - stepActiveDurationMs);
    }
    
    if (activePhase && now - lastStepTimeMs >= stepActiveDurationMs) {
        lastStepTimeMs += stepActiveDurationMs;
        uint16_t dutyCycle = flashPhaseCount % 2 == 0 ? maxPwmDuty : 0;

        Logger::logExtra("LedController", Logger::LogLevel::INFO, "%4.3f: LED turbo duty cycle '%u'", millis() / 1000.0, (unsigned int)dutyCycle);
        
        ledcWrite(channel, ScaleBrightness(dutyCycle));
        flashPhaseCount++;

        if (flashPhaseCount / 2 >= flashAmount) {
            activePhase = false;
        }
    }
}
