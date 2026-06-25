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

SW420::~SW420() {}

void SW420::Init(uint8_t pin) {
    sensPin = pin;
    pinMode(sensPin, INPUT);
}

void SW420::Init() {
    sensPin = 13;
    pinMode(sensPin, INPUT);
}



bool SW420::GetState() {
    return digitalRead(sensPin) == HIGH;
}

