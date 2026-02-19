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

// TODO Transform to constants
#define DEFAULT_FREQUENCY 5000
#define DEFAULT_RESOLUTION 8

LedController::LedController(int ledPin, int ledChannel) 
    : mode(LedMode::Standard), channel(ledChannel), lastStepTimeMs(0)
{
    ledcSetup(channel, DEFAULT_FREQUENCY, DEFAULT_RESOLUTION);
    ledcAttachPin(ledPin, channel);
    ledcWrite(channel, 0);
}

LedController::~LedController() {}

void LedController::SetMode(LedMode mode)
{
    if (this->mode != mode) {
        this->mode = mode;
        lastStepTimeMs = 0;
        Serial.printf("[INFO][LedController.cpp] %4.3f: LED mode set to '%d'\n", millis() / 1000.0, this->mode);
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
        RenderPermanentStep(255);
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

uint8_t LedController::ScaleBrightness(uint8_t value) {
    if (value == 0) return 0;
    int ledBrightness = Settings::Instance().GetLedBrightness();
    return (uint8_t)round(value * ledBrightness / 100.0f);
}

void LedController::RenderPermanentStep(uint8_t brightness) {
    static uint8_t prevBrightness = 0;
    uint8_t scaled = ScaleBrightness(brightness);
    if (scaled != prevBrightness) {
        prevBrightness = scaled;
        ledcWrite(channel, prevBrightness);
    }
}

void LedController::RenderFadeStep() {
    const unsigned long stepDurationMs = 3;
    static uint32_t dutyCycle = 1;
    static bool fadeUp = true; // fade direction

    unsigned long now = millis();
    if (lastStepTimeMs == 0) {
        dutyCycle = 1;
        lastStepTimeMs = now - stepDurationMs;
    }

    if (lastStepTimeMs + stepDurationMs <= now) {
        lastStepTimeMs += stepDurationMs;
        if (dutyCycle <= 0 || dutyCycle >= 255) {
            fadeUp = !fadeUp;
        }
        dutyCycle += (fadeUp ? 1 : -1);
        ledcWrite(channel, ScaleBrightness(dutyCycle));
    }
}

void LedController::RenderFlashStep() {
    const unsigned long stepDurationsMs[] = { 500, 100 };
    const unsigned long dutyCycles[] = { 0, 255 };
    static unsigned char phaseIdx = 0;

    unsigned long now = millis();
    if (lastStepTimeMs == 0) {
        phaseIdx = 0;
        lastStepTimeMs = now - stepDurationsMs[phaseIdx];
    }

    if (lastStepTimeMs + stepDurationsMs[phaseIdx] <= now) {
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
    Serial.printf("[INFO][LedController.cpp] %4.3f: LED turbo step %s '%d'\n", millis() / 1000.0, activePhase ? "flash" : "dark", flashPhaseCount);
    if (!activePhase && lastStepTimeMs + stepInactiveDurationMs <= now) {
        activePhase = true;
        flashPhaseCount = 0;
        lastStepTimeMs += (stepInactiveDurationMs - stepActiveDurationMs);
    }
    if (activePhase && lastStepTimeMs + stepActiveDurationMs <= now) {
        lastStepTimeMs += stepActiveDurationMs;
        int dutyCycle = flashPhaseCount % 2 == 0 ? 255 : 0;
        Serial.printf("[INFO][LedController.cpp] %4.3f: LED turbo duty cycle '%d'\n", millis() / 1000.0, dutyCycle);
        ledcWrite(channel, ScaleBrightness(dutyCycle));
        flashPhaseCount++;

        if (flashPhaseCount / 2 >= flashAmount) {
            activePhase = false;
        }
    }
}
