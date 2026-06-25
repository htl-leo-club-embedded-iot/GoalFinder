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
     * @brief Destructor for SW420.
     */
    virtual ~SW420();

    /**
     * @brief Initialize the vibration sensor hardware.
     *
     * Configures the given GPIO pin and sets its mode. Must be called
     * before performing any measurements.
     *
     * @param pin Digital input pin number to which the SW-420 is connected.
     */
    void Init(uint8_t pin);

    /**
     * @brief Initialize the vibration sensor hardware on the default pin.
     *
     * This overload uses pin 13 for backwards compatibility.
     */
    void Init();

    /**
     * @brief Get the current digital state of the sensor pin.
     *
     * @return true when the sensor pin is HIGH, false when it is LOW.
     */
    bool GetState();

  private:
    /** GPIO pin number used for the sensor. */
    uint8_t sensPin;
};
