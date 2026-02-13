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

        int GetVibrationSensorSensitivity();

        void SetVibrationSensorSensitivity(int vibrationSensorSensitivity);

        int GetBallHitDetectionDistance();

        void SetBallHitDetectionDistance(int ballHitDetectionDistance);

        LedMode GetLedMode();

        void SetLedMode(LedMode ledMode);

        /** Returns whether this is the first startup of the device. */
        bool IsFirstRun();

        /** Marks the device as having completed first run setup. */
        void SetFirstRun(bool firstRun);

        /** Resets all settings to factory defaults and restarts the device. */
        void ResetToDefaults();

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

        static const char* keyVibrationSensorSensitivity;
        static const int defaultVibrationSensorSensitivity;

        static const char* keyBallHitDetectionDistance;
        static const int defaultBallHitDetectionDistance;
        
        static const char* keyLedMode;
        static const LedMode defaultLedMode;

        static const char* keyFirstRun;
        static const bool defaultFirstRun;

        System::Settings store;
        bool modified;
};