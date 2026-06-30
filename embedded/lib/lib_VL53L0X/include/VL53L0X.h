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

#include <Adafruit_VL53L0X.h>

/**
 * @file VL53L0X.h
 * @brief Wrapper around the Adafruit VL53L0X time-of-flight distance sensor.
 *
 * Provides a simple interface for initializing the sensor on specific I2C
 * pins and reading a single distance measurement in millimeters.
 *
 * Used by the firmware to detect ball proximity. Internally uses
 * an instance of the Adafruit_VL53L0X class and a custom TwoWire configuration
 * to allow arbitrary I2C pin selection.
 */
class VL53L0X {
  public:
    /**
     * @brief Destructor. Currently a no-op.
     */
    virtual ~VL53L0X();

    /**
     * @brief Construct a new VL53L0X object.
     *
     * The constructor initializes the TwoWire instance with a dummy bus number.
     * Callers must still invoke Init() before using the sensor.
     */
    VL53L0X() : wireConfig(0) {}

    /**
     * @brief Initialize the VL53L0X sensor and I2C interface.
     *
     * @param sclPin I2C clock pin number.
     * @param sdaPin I2C data pin number.
     *
     * Configures the TwoWire object and starts the underlying Adafruit sensor.
     * Logs an error via Logger if the sensor fails to boot.
     */
    void Init(int sclPin, int sdaPin);

    /**
     * @brief Take a single distance measurement.
     *
     * @return Distance in millimeters, or -1 on error.
     *
     * The function performs a blocking ranging test and returns the value when
     * completed. A small delay is inserted after reading as a placeholder; the
     * value is not used for error handling.
     */
    int ReadSingleMillimeters();

  private:
    /** Sensor object from Adafruit library */
    Adafruit_VL53L0X sensor;
    /** Wire configuration for custom pins */
    TwoWire wireConfig;
};