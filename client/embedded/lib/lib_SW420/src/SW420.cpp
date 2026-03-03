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

#include "util/Logger.h"
#include <Arduino.h>
#include <SW420.h>

/**
 * @file SW420.cpp
 * @brief Implementation of the SW420 vibration sensor wrapper.
 *
 * The class provides initialization of the GPIO pin and a simple method to
 * measure the duration of a pulse from the SW-420 sensor using Arduino's
 * pulseIn() function. A software "sensitivity" value may be stored but is
 * currently unused.
 */

/**
 * @brief Destructor for SW420.
 *
 * No dynamic resources are managed so this is a no-op.
 */
SW420::~SW420() {}

 /**
 * @brief Initialize the vibration sensor hardware.
 *
 * Configures the default GPIO pin and sets its mode. Must be called
 * before performing any measurements.
 * @param pin The pin from which to read
 */
void SW420::Init(uint8_t pin) {
    vs = pin;
    pinMode(vs, INPUT);
}

 /**
 * @brief Initialize the vibration sensor hardware.
 *
 * Configures the default GPIO pin and sets its mode. Must be called
 * before performing any measurements.
 * @note Overloaded for backwards compatibility using 13 as the default pin
 */
void SW420::Init() {
    vs = 13;
    pinMode(vs, INPUT);
}

/**
 * @brief Perform a vibration measurement.
 *
 * @param measureTimeUs Maximum time to wait for a pulse, in microseconds.
 * @return long Length of the HIGH pulse in microseconds, or 0 if the
 *         timeout expired.
 */
long SW420::Vibration(uint64_t measureTimeUs) {
    long measurement = pulseIn(vs, HIGH, measureTimeUs);
    return measurement;
}

/**
 * @brief Set the (software) sensitivity level.
 *
 * The value is clamped to the 0–100 range. At present the sensitivity
 * parameter is not used by the sensor code; it exists for future
 * expansion or for callers that wish to track a desired threshold.
 *
 * @param sensitivity 0 (low) to 100 (high).
 */
void SW420::SetSensitivity(int sensitivity) {
    if (sensitivity < 0) {
        sensitivity = 0;
    } else if (sensitivity > 100) {
        sensitivity = 100;
    }
}