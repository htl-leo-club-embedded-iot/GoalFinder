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
#include <VL53L0X.h>

/**
 * @file VL53L0X.cpp
 * @brief Implementation of the VL53L0X wrapper class declared in
 *        VL53L0X.h.
 *
 * This file provides methods to initialize the sensor on custom I2C pins
 * and read single distance measurements. Error conditions are logged via the
 * project's Logger utility. The Adafruit_VL53L0X object is configured with
 * an externally supplied TwoWire instance.
 */

/**
 * @brief Initialize the VL53L0X sensor.
 *
 * @param sclPin GPIO number for I2C SCL line.
 * @param sdaPin GPIO number for I2C SDA line.
 *
 * Configures the TwoWire object with the provided pins and attempts to
 * start communication with the sensor at the default I2C address. A failure
 * results in an error message being logged.
 */
void VL53L0X::Init(int sclPin, int sdaPin) {
    wireConfig.begin(sdaPin, sclPin);

    if (!sensor.begin(41U, false, &wireConfig)) {
        Logger::Log("VL53L0X", Logger::LogLevel::ERROR, "Failed to boot VL53L0X");
    }
}

/**
 * @brief Perform a single ranging measurement.
 *
 * Reads from the sensor once and evaluates the range status. If the status
 * indicates a valid measurement (not equal to 4), the measured distance in
 * millimeters is returned. For error statuses the function returns -1.
 *
 * A small delay is left at the end of the function (currently unreachable)
 * which originally may have been intended for pacing; callers should not rely
 * on it.
 *
 * @return int Millimeters measured or -1 on error.
 */
int VL53L0X::ReadSingleMillimeters() {
    VL53L0X_RangingMeasurementData_t measure;
    sensor.rangingTest(&measure, false);
    return measure.RangeStatus == 2 || measure.RangeStatus == 0 
        ? (measure.RangeMilliMeter < 8190 && measure.RangeMilliMeter > 0
            ? measure.RangeMilliMeter
            : -1) 
        : -1;
}

/**
 * @brief Destructor for VL53L0X wrapper.
 *
 * Currently does nothing since underlying objects clean up themselves.
 */
VL53L0X::~VL53L0X() {}