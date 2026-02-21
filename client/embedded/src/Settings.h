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
#include <Arduino.h>
#include <Singleton.h>
#include <system/Settings.h>
#include "LedMode.h"

class Settings : public Singleton<Settings>
{
    public:
        virtual ~Settings();

        bool IsModified() const;

        void ClearModifiedState();

        /** Provides the MAC address of the WiFi interface. */
        String GetMacAddress();

        /** Provides the volume as a range from 0 to 100 percent. */
        int GetVolume();

        /** Sets the volume. The range is clipped to 0 to 100 percent.*/
        void SetVolume(int volume);

        void SetMetronomeSound(int metronomeSound);

        int GetMetronomeSound();

        void SetHitSound(int hitSound);

        int GetHitSound();

        void SetMissSound(int missSound);

        int GetMissSound();

        String GetDeviceName();

        void SetDeviceName(String deviceName);

        String GetDevicePassword();

        void SetDevicePassword(String devicePassword);

        String GetWifiPassword();

        void SetWifiPassword(String wifiPassword);

        int GetVibrationSensorSensitivity();

        void SetVibrationSensorSensitivity(int vibrationSensorSensitivity);

        int GetBallHitDetectionDistance();

        void SetBallHitDetectionDistance(int ballHitDetectionDistance);

        bool GetDistanceOnlyHitDetection();

        void SetDistanceOnlyHitDetection(bool distanceOnlyHitDetection);

        int GetLedBrightness();

        void SetLedBrightness(int ledBrightness);

        LedMode GetLedMode();

        void SetLedMode(LedMode ledMode);

        bool IsFirstRun();

        void SetFirstRun(bool firstRun);

        void ResetToDefaults();

        int GetAfterHitTimeout();

        void SetAfterHitTimeout(int timeout);

    private:
		friend class Singleton<Settings>;
        /** Singleton constructor */
        Settings();
        void SetModified();

        static const char* keyVolume;
        static const int defaultVolume;

        static const char* keyMetronomeSound;
        static const int defaultMetronomeSound;

        static const char* keyHitSound;
        static const int defaultHitSound;

        static const char* keyMissSound;
        static const int defaultMissSound;

        static const char* keyDeviceName;
        static const String defaultDeviceName;

        static const char* keyDevicePassword;
        static const String defaultDevicePassword;

        static const char* keyWifiPassword;
        static const String defaultWifiPassword;

        static const char* keyVibrationSensorSensitivity;
        static const int defaultVibrationSensorSensitivity;

        static const char* keyBallHitDetectionDistance;
        static const int defaultBallHitDetectionDistance;

        static const char* keyDistanceOnlyHitDetection;
        static const bool defaultDistanceOnlyHitDetection;

        static const char* keyLedBrightness;
        static const int defaultLedBrightness;

        static const char* keyLedMode;
        static const LedMode defaultLedMode;

        static const char* keyFirstRun;
        static const bool defaultFirstRun;

        static const char* keyAfterHitTimeout;
        static const int defaultAfterHitTimeout;

        System::Settings store;
        bool modified;
};