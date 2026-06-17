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

constexpr uint8_t HIT_FLASH_COUNT = 6;
constexpr unsigned long HIT_FLASH_ON_MS = 80;
constexpr unsigned long HIT_FLASH_OFF_MS = 80;

constexpr unsigned long MISS_FADE_DURATION_MS = 900;

constexpr unsigned long SHOT_BREATHE_CYCLE_MS = 1200;
constexpr float SHOT_BREATHE_MAX_FRACTION = 0.5f;
}

LedController::LedController(int ledPin, int ledChannel) 
    : mode(LedMode::Standard), channel(ledChannel), lastStepTimeMs(0), lastAppliedDuty(0),
      _eventActive(false), _eventType(EventEffect::None), _savedMode(LedMode::Standard),
      _eventPhase(0), _eventStartTime(0), _eventBaseDuty(0)
{
    ledcSetup(channel, defaultFrequency, defaultResolution);
    ledcAttachPin(ledPin, channel);
    ledcWrite(channel, 0);
}

LedController::~LedController() {}

void LedController::SetMode(LedMode mode)
{
    if (_eventActive) {
        ClearEvent();
    }
    if (this->mode != mode) {
        this->mode = mode;
        lastStepTimeMs = 0;
        lastAppliedDuty = 0xFFFF;
        Logger::Log("LedController", Logger::LogLevel::OK, "%4.3f: LED mode set to '%d'", millis() / 1000.0, this->mode);
    }
}

LedMode LedController::GetMode()
{
    return mode;
}

void LedController::ClearEvent()
{
    _eventActive = false;
    _eventType = EventEffect::None;
    _eventPhase = 0;
    _eventStartTime = 0;
    lastStepTimeMs = 0;
    lastAppliedDuty = 0xFFFF;
}

void LedController::OnHit()
{
    _savedMode = mode;
    _eventActive = true;
    _eventType = EventEffect::Hit;
    _eventPhase = 0;
    _eventStartTime = millis();
    _eventBaseDuty = maxPwmDuty;
    lastStepTimeMs = 0;
    lastAppliedDuty = 0xFFFF;
    Logger::Log("LedController", Logger::LogLevel::OK, "%4.3f: LED hit event started", millis() / 1000.0);
}

void LedController::OnMiss()
{
    _savedMode = mode;
    _eventActive = true;
    _eventType = EventEffect::Miss;
    _eventPhase = 0;
    _eventStartTime = millis();
    _eventBaseDuty = maxPwmDuty;
    lastStepTimeMs = 0;
    lastAppliedDuty = 0xFFFF;
    Logger::Log("LedController", Logger::LogLevel::OK, "%4.3f: LED miss event started", millis() / 1000.0);
}

void LedController::OnShotDetected()
{
    if (!_eventActive || _eventType != EventEffect::Shot) {
        _savedMode = mode;
        _eventActive = true;
        _eventType = EventEffect::Shot;
        _eventPhase = 0;
        _eventStartTime = millis();
        _eventBaseDuty = maxPwmDuty;
        lastStepTimeMs = 0;
        lastAppliedDuty = 0xFFFF;
        Logger::Log("LedController", Logger::LogLevel::OK, "%4.3f: LED shot event started", millis() / 1000.0);
    }
}

void LedController::Loop() 
{
    if (_eventActive) {
        if (_eventType == EventEffect::Hit) {
            RenderHitFlash();
        } else if (_eventType == EventEffect::Miss) {
            RenderMissFade();
        } else if (_eventType == EventEffect::Shot) {
            RenderShotBreathing();
        }
    } else {
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
}

uint16_t LedController::ScaleBrightness(uint16_t value) {
    uint16_t scaled = 0;
    if (value != 0) {
        int ledBrightness = Settings::GetInstance()->GetLedBrightness();
        ledBrightness = max(min(ledBrightness, 100), 0);
        scaled = (uint16_t)roundf(value * ledBrightness / 100.0f);
    }
    return scaled;
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
    const unsigned long stepDurationsMs[] = { static_cast<unsigned long>(Settings::GetInstance()->GetMetronomeTiming()) / 2, static_cast<unsigned long>(Settings::GetInstance()->GetMetronomeTiming()) / 5 };
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
    const unsigned long stepInactiveDurationMs = Settings::GetInstance()->GetMetronomeTiming() / 3;
    const unsigned long stepActiveDurationMs = Settings::GetInstance()->GetMetronomeTiming() / 12;
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

    Logger::LogExtra("LedController", Logger::LogLevel::INFO, "%4.3f: LED turbo step %s '%lu'", millis() / 1000.0, activePhase ? "flash" : "dark", (unsigned long)flashPhaseCount);
    
    if (!activePhase && now - lastStepTimeMs >= stepInactiveDurationMs) {
        activePhase = true;
        flashPhaseCount = 0;
        lastStepTimeMs += (stepInactiveDurationMs - stepActiveDurationMs);
    }
    
    if (activePhase && now - lastStepTimeMs >= stepActiveDurationMs) {
        lastStepTimeMs += stepActiveDurationMs;
        uint16_t dutyCycle = flashPhaseCount % 2 == 0 ? maxPwmDuty : 0;

        Logger::LogExtra("LedController", Logger::LogLevel::INFO, "%4.3f: LED turbo duty cycle '%u'", millis() / 1000.0, (unsigned int)dutyCycle);
        
        ledcWrite(channel, ScaleBrightness(dutyCycle));
        flashPhaseCount++;

        if (flashPhaseCount / 2 >= flashAmount) {
            activePhase = false;
        }
    }
}

void LedController::RenderHitFlash() {
    const unsigned long totalPhases = HIT_FLASH_COUNT * 2;

    unsigned long now = millis();
    bool effectDone = false;

    if (lastStepTimeMs == 0) {
        lastStepTimeMs = _eventStartTime;
        _eventPhase = 0;
        uint16_t duty = _eventBaseDuty;
        ledcWrite(channel, ScaleBrightness(duty));
        lastAppliedDuty = duty;
    } else if (_eventPhase >= totalPhases) {
        effectDone = true;
    } else {
        unsigned long phaseDuration = (_eventPhase % 2 == 0) ? HIT_FLASH_ON_MS : HIT_FLASH_OFF_MS;

        if (now - lastStepTimeMs >= phaseDuration) {
            _eventPhase++;
            lastStepTimeMs = now;

            if (_eventPhase >= totalPhases) {
                ledcWrite(channel, 0);
                lastAppliedDuty = 0;
                effectDone = true;
            } else {
                uint16_t duty = (_eventPhase % 2 == 0) ? _eventBaseDuty : 0;
                ledcWrite(channel, ScaleBrightness(duty));
                lastAppliedDuty = duty;
            }
        }
    }

    if (effectDone) {
        ClearEvent();
    }
}

void LedController::RenderMissFade() {
    unsigned long now = millis();
    bool effectDone = false;

    if (lastStepTimeMs == 0) {
        lastStepTimeMs = now;
        _eventStartTime = now;
        _eventBaseDuty = maxPwmDuty;
        uint16_t scaled = ScaleBrightness(_eventBaseDuty);
        ledcWrite(channel, scaled);
        lastAppliedDuty = _eventBaseDuty;
    } else {
        unsigned long elapsedMs = now - _eventStartTime;
        if (elapsedMs >= MISS_FADE_DURATION_MS) {
            ledcWrite(channel, 0);
            lastAppliedDuty = 0;
            effectDone = true;
        } else {
            float fraction = 1.0f - (float)elapsedMs / (float)MISS_FADE_DURATION_MS;
            uint16_t duty = (uint16_t)roundf((float)_eventBaseDuty * fraction);
            uint16_t scaled = ScaleBrightness(duty);

            if (scaled != lastAppliedDuty) {
                lastAppliedDuty = scaled;
                ledcWrite(channel, scaled);
            }
        }
    }

    if (effectDone) {
        ClearEvent();
    }
}

void LedController::RenderShotBreathing() {
    unsigned long now = millis();
    if (lastStepTimeMs == 0) {
        lastStepTimeMs = now;
        _eventStartTime = now;
    }

    const unsigned long elapsedMs = now - _eventStartTime;
    const unsigned long phaseMs = elapsedMs % SHOT_BREATHE_CYCLE_MS;
    const float phase = (tau * phaseMs) / SHOT_BREATHE_CYCLE_MS;
    const float normalized = 0.5f - 0.5f * cosf(phase);
    const float shotLevel = normalized * SHOT_BREATHE_MAX_FRACTION;
    const uint16_t dutyCycle = (uint16_t)roundf(shotLevel * maxPwmDuty);
    const uint16_t scaledDuty = ScaleBrightness(dutyCycle);

    if (scaledDuty != lastAppliedDuty) {
        lastAppliedDuty = scaledDuty;
        ledcWrite(channel, lastAppliedDuty);
    }
}