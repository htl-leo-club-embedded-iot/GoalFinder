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

#include <stdint.h>

/**
 * @file SW420.h
 * @brief Simple interface for the SW-420 vibration sensor.
 *
 * This class encapsulates the Arduino-specific logic required to read a
 * vibration sensor attached to a digital input pin. The caller may request a
 * pulse length measurement for a specified timeout and adjust an abstract
 * "sensitivity" parameter (currently only clamped, no hardware effect).
 */
class SW420 {
  public:
    /**
     * @brief Destructor.
     */
    virtual ~SW420();

    /**
     * @brief Initialize the vibration sensor hardware on a specific pin.
     *
     * Configures the provided GPIO pin and sets its mode. Must be called
     * before performing any measurements.
     *
     * @param pin Digital input pin number to which the SW-420 is connected.
     * @param sensitivity The sensitivity of the sensor (1-100)
     */
    void Init(uint8_t pin, uint8_t sensitivity);

    /**
     * @brief Initialize the vibration sensor hardware on the default pin.
     *
     * This overload exists for backwards compatibility and simply calls
     * Init(13).
     * @param sensitivity The sensitivity of the sensor (1-100)
     */
    void Init(uint8_t sensitivity);

    /**
     * @brief Perform a vibration measurement.
     *
     * @param measureTimeUs Maximum time to wait for a pulse, in microseconds.
     * @return long Length of the HIGH pulse in microseconds, or 0 if the
     *         timeout expired.
     */
    long Vibration(uint64_t measureTimeUs);

    /**
     * @brief Set the (software) sensitivity level.
     *
     * The value is clamped to the 0–100 range. At present the sensitivity
     * parameter is not used by the sensor code; it exists for future
     * expansion or for callers that wish to track a desired threshold.
     *
     * @param sensitivity 0 (low) to 100 (high).
     */
    void SetSensitivity(uint8_t sensitivity);

  private:
    /** GPIO pin number used for the sensor. */
    uint8_t sensPin; 

    /** The sensitivity of the sensor */
    uint8_t sensitivity;
};
