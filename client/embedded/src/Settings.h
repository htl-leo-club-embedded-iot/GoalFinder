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
        /** Destructor. */
        virtual ~Settings();

        /** Returns true if settings have changed since the last save. */
        bool IsModified() const;

        /** Clears the modified state for the current settings. */
        void ClearModifiedState();

        /** Returns the configured MAC address. */
        String GetMacAddress();

        /** Returns the current system volume level. */
        int GetVolume();
        /** Sets the current system volume level. */
        void SetVolume(int volume);

        /** Sets the selected metronome sound index. */
        void SetMetronomeSound(int metronomeSound);
        /** Returns the selected metronome sound index. */
        int GetMetronomeSound();

        /** Sets the selected hit sound index. */
        void SetHitSound(int hitSound);
        /** Returns the selected hit sound index. */
        int GetHitSound();

        /** Sets the selected miss sound index. */
        void SetMissSound(int missSound);
        /** Returns the selected miss sound index. */
        int GetMissSound();

        /** Sets the selected waiting sound index. */
        void SetWaitingSound(int waitingSound);
        /** Returns the selected waiting sound index. */
        int GetWaitingSound();

        /** Sets the time between metronome ticks (in ms) */
        void SetMetronomeTiming(int delayMS);
        /** Returns the time between metronome ticks (in ms) */
        int GetMetronomeTiming();

        /** Returns the configured device name. */
        String GetDeviceName();
        /** Sets the device name. */
        void SetDeviceName(String deviceName);

        /** Returns the configured device password. */
        String GetDevicePassword();
        /** Sets the device password. */
        void SetDevicePassword(String devicePassword);
        
        /** Returns the configured Wi-Fi password. */
        String GetWifiPassword();
        /** Sets the Wi-Fi password. */
        void SetWifiPassword(String wifiPassword);

        /** Returns the vibration sensor sensitivity value. */
        int GetVibrationSensorSensitivity();
        /** Sets the vibration sensor sensitivity value. */
        void SetVibrationSensorSensitivity(int vibrationSensorSensitivity);

        /** Returns the ball hit detection distance in millimeters. */
        int GetBallHitDetectionDistance();
        /** Sets the ball hit detection distance in millimeters. */
        void SetBallHitDetectionDistance(int ballHitDetectionDistance);

        /** Returns true when only distance-based hit detection is enabled. */
        bool GetDistanceOnlyHitDetection();
        /** Enables or disables distance-only hit detection. */
        void SetDistanceOnlyHitDetection(bool distanceOnlyHitDetection);

        /** Returns the configured LED brightness. */
        int GetLedBrightness();
        /** Sets the configured LED brightness. */
        void SetLedBrightness(int ledBrightness);

        /** Returns the current LED mode. */
        LedMode GetLedMode();
        /** Sets the current LED mode. */
        void SetLedMode(LedMode ledMode);

        /** Returns true if this is the first run after reset. */
        bool IsFirstRun();
        /** Sets the first-run flag. */
        void SetFirstRun(bool firstRun);

        /** Resets all settings to their default values. */
        void ResetToDefaults();

        /** Returns the configured after-hit timeout in seconds. */
        int GetAfterHitTimeout();
        /** Sets the after-hit timeout in seconds. */
        void SetAfterHitTimeout(int timeout);

        /** Returns whether the last update was successful. */
        bool GetUpdateSuccess();
        /** Sets the update success flag. */
        void SetUpdateSuccess(bool success);

        /** Returns whether extra logging is enabled. */
        bool GetExtraLog();
        /** Enables or disables extra logging. */
        void SetExtraLog(bool enabled);

        /** Returns whether external network mode is enabled. */
        bool GetUseExternalNW();
        /** Enables or disables external network mode. */
        void SetUseExternalNW(bool enable);

        /** Returns the external network SSID. */
        String GetExternalNW_SSID();
        /** Sets the external network SSID. */
        void SetExternalNW_SSID(String ssid);

        /** Returns the external network password. */
        String GetExternalNW_PWD();
        /** Sets the external network password. */
        void SetExternalNW_PWD(String pwd);

        /** Returns if DHCP is to be used for external networks */
        bool GetExternalNWE_UseDHCP();
        /** Sets if DHCP is to be used for external networks */
        void SetExternalNWE_UseDHCP(bool useDHCP);

        /** Returns the manually configured ip address for external networks */
        String GetExternalNW_IP();
        /** Sets the manually configured ip address for external networks */
        void SetExternalNW_IP(String ip);

        /** Returns the manually configured subnet mask for external networks */
        String GetExternalNW_SNM();
        /** Sets the manually configured subnet mask for external networks */
        void SetExternalNW_SNM(String snm);

        /** Returns the manually configured default gateway for external networks */
        String GetExternalNW_DFG();
        /** Sets the manually configured default gateway for external networks */
        void SetExternalNW_DFG(String dfg);

        /** Returns the manually configured DNS server address for external networks */
        String GetExternalNW_DNSIP();
        /** Sets the manually configured DNS server address for external networks */
        void SetExternalNW_DNSIP(String dnsIP);

        /** Returns the external network authentication mode. */
        String GetExternalNW_AuthMode();
        /** Sets the external network authentication mode. */
        void SetExternalNW_AuthMode(String authMode);

        /** Returns the enterprise identity for external network authentication. */
        String GetExternalNW_EnterpriseIdentity();
        /** Sets the enterprise identity for external network authentication. */
        void SetExternalNW_EnterpriseIdentity(String identity);

        /** Returns the enterprise username for external network authentication. */
        String GetExternalNW_EnterpriseUsername();
        /** Sets the enterprise username for external network authentication. */
        void SetExternalNW_EnterpriseUsername(String username);

        /** Returns the enterprise anonymous identity for external network authentication. */
        String GetExternalNW_EnterpriseAnonymousIdentity();
        /** Sets the enterprise anonymous identity for external network authentication. */
        void SetExternalNW_EnterpriseAnonymousIdentity(String anonymousIdentity);

        /** Returns the enterprise password for external network authentication. */
        String GetExternalNW_EnterprisePassword();
        /** Sets the enterprise password for external network authentication. */
        void SetExternalNW_EnterprisePassword(String password);

        /** Returns the enterprise phase 2 method for external network authentication. */
        String GetExternalNW_EnterprisePhase2Method();
        /** Sets the enterprise phase 2 method for external network authentication. */
        void SetExternalNW_EnterprisePhase2Method(String phase2Method);

        /** Returns the enterprise CA certificate for external network authentication. */
        String GetExternalNW_EnterpriseCaCertificate();
        /** Sets the enterprise CA certificate for external network authentication. */
        void SetExternalNW_EnterpriseCaCertificate(String caCertificate);

        /** Returns the enterprise client certificate for external network authentication. */
        String GetExternalNW_EnterpriseClientCertificate();
        /** Sets the enterprise client certificate for external network authentication. */
        void SetExternalNW_EnterpriseClientCertificate(String clientCertificate);

        /** Returns the enterprise client private key for external network authentication. */
        String GetExternalNW_EnterpriseClientPrivateKey();
        /** Sets the enterprise client private key for external network authentication. */
        void SetExternalNW_EnterpriseClientPrivateKey(String clientPrivateKey);

        /** Returns the configured device IP address. */
        String GetDeviceIpAddress();
        /** Sets the configured device IP address. */
        void SetDeviceIpAddress(String ip);

        /** Returns the configured subnet mask. */
        String GetSubnetMask();
        /** Sets the configured subnet mask. */
        void SetSubnetMask(String mask);

        /** Returns whether advanced settings are enabled. */
        bool AdvancedSettingsEnabled();
        /** Enables or disables advanced settings. */
        void SetAdvancedSettingsEnabled(bool enable);

        /** Returns whether DNS is enabled. */
        bool DNSEnabled();
        /** Enables or disables DNS. */
        void SetDNSEnabled(bool enable);

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

        static const char* keyWaitingSound;
        static const int defaultWaitingSound;

        static const char* keyMissSound;
        static const int defaultMissSound;

        static const char* keyMetronomeTiming;
        static const int defaultMetronomeTiming;

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

        static const char* keyUpdateSuccess;
        static const bool defaultUpdateSuccess;

        static const char* keyExtraLog;
        static const bool defaultExtraLog;

        static const char* keyUseExternalNW;
        static const bool defaultUseExternalNW;

        static const char* keyExternalNW_SSID;
        static const String defaultExternalNW_SSID;

        static const char* keyExternalNW_PWD;
        static const String defaultExternalNW_PWD;

        static const char* keyExternalNW_UseDHCP;
        static const bool defaultExternalNW_UseDHCP;

        static const char* keyExternalNW_IP;
        static const String defaultExternalNW_IP;
        
        static const char* keyExternalNW_SNM;
        static const String defaultExternalNW_SNM;

        static const char* keyExternalNW_DFG;
        static const String defaultExternalNW_DFG;

        static const char* keyExternalNW_DNSIP;
        static const String defaultExternalNW_DNSIP;

        static const char* keyExternalNW_AuthMode;
        static const String defaultExternalNW_AuthMode;

        static const char* keyExternalNW_EnterpriseIdentity;
        static const String defaultExternalNW_EnterpriseIdentity;

        static const char* keyExternalNW_EnterpriseUsername;
        static const String defaultExternalNW_EnterpriseUsername;

        static const char* keyExternalNW_EnterpriseAnonymousIdentity;
        static const String defaultExternalNW_EnterpriseAnonymousIdentity;

        static const char* keyExternalNW_EnterprisePassword;
        static const String defaultExternalNW_EnterprisePassword;

        static const char* keyExternalNW_EnterprisePhase2Method;
        static const String defaultExternalNW_EnterprisePhase2Method;

        static const char* keyExternalNW_EnterpriseCaCertificate;
        static const String defaultExternalNW_EnterpriseCaCertificate;

        static const char* keyExternalNW_EnterpriseClientCertificate;
        static const String defaultExternalNW_EnterpriseClientCertificate;

        static const char* keyExternalNW_EnterpriseClientPrivateKey;
        static const String defaultExternalNW_EnterpriseClientPrivateKey;

        static const char* keyDeviceIP;
        static const String defaultDeviceIP;

        static const char* keySubnetMask;
        static const String defaultSubnetMask;

        static const char* keyEnableAdvancedSettings;
        static const bool defaultEnableAdvancedSettings;

        static const char* keyEnableDNS;
        static const bool defaultEnableDNS;

        System::Settings store;
        bool modified;
};