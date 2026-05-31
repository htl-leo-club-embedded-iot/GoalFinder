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

#include "Settings.h"
#include "WiFi.h"
#include <math.h>
#include "GoalfinderApp.h"
#include "web/DNSServer.h"
#include "util/Logger.h"
#include "Config.h"

const char* Settings::keyVolume = "volume";
const int Settings::defaultVolume = 25;

const char* Settings::keyMetronomeSound = "metronomeSound";
const int Settings::defaultMetronomeSound = 0;

const char* Settings::keyHitSound = "hitSound";
const int Settings::defaultHitSound = 0;

const char* Settings::keyMissSound = "missSound";
const int Settings::defaultMissSound = 0;

const char* Settings::keyWaitingSound = "missSound";
const int Settings::defaultWaitingSound = 0;

const char* Settings::keyMetronomeTiming = "metSoundDelay";
const int Settings::defaultMetronomeTiming = 1500;

const char* Settings::keyDeviceName = "deviceName";
const String Settings::defaultDeviceName = "GoalFinder 01";

const char* Settings::keyDevicePassword = "devicePassword";
const String Settings::defaultDevicePassword = emptyString;

const char* Settings::keyWifiPassword = "wifiPassword";
const String Settings::defaultWifiPassword = emptyString;

const char* Settings::keyVibrationSensorSensitivity = "shotSensitivity";
const int Settings::defaultVibrationSensorSensitivity = 100;

const char* Settings::keyBallHitDetectionDistance = "ballHitDetDist";
const int Settings::defaultBallHitDetectionDistance = 180;

const char* Settings::keyDistanceOnlyHitDetection = "distOnlyHitDet";
const bool Settings::defaultDistanceOnlyHitDetection = false;

const char* Settings::keyLedBrightness = "ledBrightness";
const int Settings::defaultLedBrightness = 100;

const char* Settings::keyLedMode = "ledMode";
const LedMode Settings::defaultLedMode = LedMode::Flash;

const char* Settings::keyFirstRun = "firstRun";
const bool Settings::defaultFirstRun = true;

const char* Settings::keyAfterHitTimeout = "afterHitTimeout";
const int Settings::defaultAfterHitTimeout = 5;

const char* Settings::keyUpdateSuccess = "updateSuccess";
const bool Settings::defaultUpdateSuccess = false;

const char* Settings::keyExtraLog = "extraLog";
const bool Settings::defaultExtraLog = false;

const char* Settings::keyUseExternalNW = "extNW";
const bool Settings::defaultUseExternalNW = false;

const char* Settings::keyExternalNW_SSID = "extNWSSID";
const String Settings::defaultExternalNW_SSID = emptyString;

const char* Settings::keyExternalNW_PWD = "extNWPWD";
const String Settings::defaultExternalNW_PWD = emptyString;

const char* Settings::keyExternalNW_UseDHCP = "extNWUseDHCP";
const bool Settings::defaultExternalNW_UseDHCP = true;

const char* Settings::keyExternalNW_IP = "extNWIP";
const String Settings::defaultExternalNW_IP = "192.168.4.1";

const char* Settings::keyExternalNW_SNM = "extNWSNM";
const String Settings::defaultExternalNW_SNM = "255.255.0.0";

const char* Settings::keyExternalNW_DFG = "extNWDFG";
const String Settings::defaultExternalNW_DFG = "192.168.0.0";

const char* Settings::keyExternalNW_DNSIP = "extNWDNSIP";
const String Settings::defaultExternalNW_DNSIP = "192.168.0.1";

const char* Settings::keyExternalNW_AuthMode = "extNWAuthMode";
const String Settings::defaultExternalNW_AuthMode = "wpa2-personal";

const char* Settings::keyExternalNW_EnterpriseIdentity = "extNWIdent";
const String Settings::defaultExternalNW_EnterpriseIdentity = emptyString;

const char* Settings::keyExternalNW_EnterpriseUsername = "extNWUser";
const String Settings::defaultExternalNW_EnterpriseUsername = emptyString;

const char* Settings::keyExternalNW_EnterpriseAnonymousIdentity = "extNWAnonId";
const String Settings::defaultExternalNW_EnterpriseAnonymousIdentity = emptyString;

const char* Settings::keyExternalNW_EnterprisePassword = "extNWEntPWD";
const String Settings::defaultExternalNW_EnterprisePassword = emptyString;

const char* Settings::keyExternalNW_EnterprisePhase2Method = "extNWEntPhase2";
const String Settings::defaultExternalNW_EnterprisePhase2Method = "auto";

const char* Settings::keyExternalNW_EnterpriseCaCertificate = "extNWEntCACert";
const String Settings::defaultExternalNW_EnterpriseCaCertificate = emptyString;

const char* Settings::keyExternalNW_EnterpriseClientCertificate = "extNWClientCrt";
const String Settings::defaultExternalNW_EnterpriseClientCertificate = emptyString;

const char* Settings::keyExternalNW_EnterpriseClientPrivateKey = "extNWClientKey";
const String Settings::defaultExternalNW_EnterpriseClientPrivateKey = emptyString;

const char* Settings::keyDeviceIP = "deviceIP";
const String Settings::defaultDeviceIP = "192.168.4.1";

const char* Settings::keySubnetMask = "subnetMask";
const String Settings::defaultSubnetMask = "255.255.255.0";

const char* Settings::keyEnableAdvancedSettings = "advSettingsEn";
const bool Settings::defaultEnableAdvancedSettings = false;

const char* Settings::keyEnableDNS = "enableDNS";
const bool Settings::defaultEnableDNS = true;

namespace {
int ClampSoundIndex(int value, int soundCount) {
	int maxIndex = max(soundCount - 1, 0);
	return max(min(value, maxIndex), 0);
}

bool IsValidExternalNetworkAuthMode(const String& authMode) {
	bool isValid =
		authMode == "open" ||
		authMode == "wpa2-personal" ||
		authMode == "wpa2-enterprise";

	return isValid;
}

bool IsValidExternalNetworkPhase2Method(const String& phase2Method) {
	bool isValid =
		phase2Method == "auto" ||
		phase2Method == "mschapv2" ||
		phase2Method == "gtc";

	return isValid;
}

String NormalizeExternalNetworkAuthMode(String authMode) {
	authMode.trim();
	authMode.toLowerCase();

	if (!IsValidExternalNetworkAuthMode(authMode)) {
		authMode = "wpa2-personal";
	}

	return authMode;
}

String NormalizeExternalNetworkPhase2Method(String phase2Method) {
	phase2Method.trim();
	phase2Method.toLowerCase();

	if (!IsValidExternalNetworkPhase2Method(phase2Method)) {
		phase2Method = "auto";
	}

	return phase2Method;
}
}
	
Settings::Settings() :
    Singleton<Settings>(),
	store(),
	modified(false) {
	if (!store.Begin("app_prefs")) {
		Logger::Log("Settings", Logger::LogLevel::ERROR, "Failed to initialize preferences namespace 'app_prefs'");
		return;
	}

	if (!store.IsKey(keyDeviceIP)) {
		store.PutString(keyDeviceIP, defaultDeviceIP);
		Logger::Log("Settings", Logger::LogLevel::WARN, "Missing key '%s'. Using default '%s'", keyDeviceIP, defaultDeviceIP.c_str());
	}

	if (!store.IsKey(keySubnetMask)) {
		store.PutString(keySubnetMask, defaultSubnetMask);
		Logger::Log("Settings", Logger::LogLevel::WARN, "Missing key '%s'. Using default '%s'", keySubnetMask, defaultSubnetMask.c_str());
	}
	}

	Settings::~Settings() {
}

String Settings::GetMacAddress() {
	return String(WiFi.macAddress());
}

int Settings::GetVolume() {
	return store.GetInt(keyVolume, defaultVolume);
}

void Settings::SetVolume(int volume) {
	volume = max(min(volume, 100), 0);
	store.PutInt(keyVolume, volume);
	SetModified();
}

void Settings::SetMetronomeSound(int metronomeSound) {
	metronomeSound = ClampSoundIndex(metronomeSound, nTickSounds);
	store.PutInt(keyMetronomeSound, metronomeSound);
	SetModified();
}

int Settings::GetMetronomeSound() {
	return ClampSoundIndex(store.GetInt(keyMetronomeSound, defaultMetronomeSound), nTickSounds);
}

void Settings::SetHitSound(int hitSound) {
	hitSound = ClampSoundIndex(hitSound, nHitSounds);
	store.PutInt(keyHitSound, hitSound);
	SetModified();
}

int Settings::GetHitSound() {
	return ClampSoundIndex(store.GetInt(keyHitSound, defaultHitSound), nHitSounds);
}

void Settings::SetMissSound(int missSound) {
	missSound = ClampSoundIndex(missSound, nMissSounds);
	store.PutInt(keyMissSound, missSound);
	SetModified();
}

void Settings::SetWaitingSound(int waitingSound) {
	waitingSound = ClampSoundIndex(waitingSound, nWaitingSounds);
	store.PutInt(keyWaitingSound, waitingSound);
	SetModified();
}

int Settings::GetMissSound() {
	return ClampSoundIndex(store.GetInt(keyMissSound, defaultMissSound), nMissSounds);
}

int Settings::GetWaitingSound() {
	return ClampSoundIndex(store.GetInt(keyWaitingSound, defaultWaitingSound), nWaitingSounds);
}

void Settings::SetMetronomeTiming(int delayMS) {
	store.PutInt(keyMetronomeTiming, delayMS);
	SetModified();
}

int Settings::GetMetronomeTiming() {
	return store.GetInt(keyMetronomeTiming, defaultMetronomeTiming);
}

bool Settings::IsModified() const {
	return modified;
}

void Settings::SetModified() {
	modified = true;
}

void Settings::ClearModifiedState() {
	modified = false;
}

String Settings::GetDeviceName() {
	return store.GetString(keyDeviceName, defaultDeviceName);
}

void Settings::SetDeviceName(String deviceName) {
	if(deviceName.isEmpty()) {
		deviceName = defaultDeviceName;
	}
	
	store.PutString(keyDeviceName, deviceName);
	SetModified(); 
}

String Settings::GetDevicePassword() {
	return store.IsKey(keyDevicePassword) ? store.GetString(keyDevicePassword, defaultDevicePassword) : defaultDevicePassword;
}

void Settings::SetDevicePassword(String devicePassword) {
	devicePassword.trim();

	if(devicePassword.isEmpty()) {
		if (store.IsKey(keyDevicePassword)) {
			store.Remove(keyDevicePassword);
			SetModified();
		}
	} else if (devicePassword.length() >= 8 && devicePassword.length() < 63) {
		store.PutString(keyDevicePassword, devicePassword);
		SetModified();
	} else {
		Logger::Log("Settings", Logger::LogLevel::WARN, "Ignoring invalid device password length. Expected 8-63 characters.");
	}
}

String Settings::GetWifiPassword() {
	return store.IsKey(keyWifiPassword) ? store.GetString(keyWifiPassword, defaultWifiPassword) : defaultWifiPassword;
}

void Settings::SetWifiPassword(String wifiPassword) {
	wifiPassword.trim();

	if(wifiPassword.isEmpty()) {
		if (store.IsKey(keyWifiPassword)) {
			store.Remove(keyWifiPassword);
			SetModified();
		}
	} else if (wifiPassword.length() >= 8 && wifiPassword.length() < 63) {
		store.PutString(keyWifiPassword, wifiPassword);
		SetModified();		
	} else {
		Logger::Log("Settings", Logger::LogLevel::WARN, "Ignoring invalid WiFi password length. Expected 8-63 characters.");
	}
}

int Settings::GetVibrationSensorSensitivity() {
	return store.GetInt(keyVibrationSensorSensitivity, defaultVibrationSensorSensitivity);
}

void Settings::SetVibrationSensorSensitivity(int vibrationSensorSensitivity) {
	vibrationSensorSensitivity = max(min(vibrationSensorSensitivity, 100), 0);
	store.PutInt(keyVibrationSensorSensitivity, vibrationSensorSensitivity);
	SetModified();
};

int Settings::GetBallHitDetectionDistance()  {
	return store.GetInt(keyBallHitDetectionDistance, defaultBallHitDetectionDistance);
}

void Settings::SetBallHitDetectionDistance(int ballHitDetectionDistance) {
	ballHitDetectionDistance = max(min(ballHitDetectionDistance, 600), 100);
	store.PutInt(keyBallHitDetectionDistance, ballHitDetectionDistance);
	SetModified();
}

bool Settings::GetDistanceOnlyHitDetection() {
	return (bool)store.GetInt(keyDistanceOnlyHitDetection, (int)defaultDistanceOnlyHitDetection);
}

void Settings::SetDistanceOnlyHitDetection(bool distanceOnlyHitDetection) {
	store.PutInt(keyDistanceOnlyHitDetection, (int)distanceOnlyHitDetection);
	SetModified();
}

int Settings::GetLedBrightness() {
	return store.GetInt(keyLedBrightness, defaultLedBrightness);
}

void Settings::SetLedBrightness(int ledBrightness) {
	ledBrightness = max(min(ledBrightness, 100), 0);
	store.PutInt(keyLedBrightness, ledBrightness);
	SetModified();
}

LedMode Settings::GetLedMode() {
	return (LedMode)store.GetInt(keyLedMode, (int)defaultLedMode);
};

void Settings::SetLedMode(LedMode ledMode) {
	store.PutInt(keyLedMode, (int)ledMode);
	SetModified();
};

bool Settings::IsFirstRun() {
	return (bool)store.GetInt(keyFirstRun, (int)defaultFirstRun);
}

void Settings::SetFirstRun(bool firstRun) {
	store.PutInt(keyFirstRun, (int)firstRun);
	SetModified();
}
  
void Settings::ResetToDefaults() {
	store.Clear();
	ESP.restart();
}

int Settings::GetAfterHitTimeout() {
	return store.GetInt(keyAfterHitTimeout, defaultAfterHitTimeout);
}

void Settings::SetAfterHitTimeout(int timeout) {
	timeout = max(min(timeout, 60), 0);
	store.PutInt(keyAfterHitTimeout, timeout);
	SetModified();
}

bool Settings::GetUpdateSuccess() {
	return (bool)store.GetInt(keyUpdateSuccess, (int)defaultUpdateSuccess);
}

void Settings::SetUpdateSuccess(bool success) {
	store.PutInt(keyUpdateSuccess, (int)success);
}

bool Settings::GetExtraLog() {
	return (bool)store.GetInt(keyExtraLog, (int)defaultExtraLog);
}

void Settings::SetExtraLog(bool enabled) {
	store.PutInt(keyExtraLog, (int)enabled);
	SetModified();
}

bool Settings::GetUseExternalNW() {
	return (bool)store.GetInt(keyUseExternalNW, (int)defaultUseExternalNW);
}


void Settings::SetUseExternalNW(bool enable) {
	store.PutInt(keyUseExternalNW, (int)enable);
	SetModified();
}

String Settings::GetExternalNW_SSID() {
	return store.IsKey(keyExternalNW_SSID) ? store.GetString(keyExternalNW_SSID, defaultExternalNW_SSID) : defaultExternalNW_SSID;
}


void Settings::SetExternalNW_SSID(String ssid) {
	ssid.trim();
	store.PutString(keyExternalNW_SSID, ssid);
	SetModified();
}

String Settings::GetExternalNW_PWD() {
	return store.IsKey(keyExternalNW_PWD) ? store.GetString(keyExternalNW_PWD, defaultExternalNW_PWD) : defaultExternalNW_PWD;
}

void Settings::SetExternalNW_PWD(String pwd) {
	pwd.trim();

	if(pwd.isEmpty())
	{
		if (store.IsKey(keyExternalNW_PWD)) {
			store.Remove(keyExternalNW_PWD);
			SetModified();
		}
	} else if (pwd.length() >= 8 && pwd.length() < 64) {
		store.PutString(keyExternalNW_PWD, pwd);
		SetModified();
	} else {
		Logger::Log("Settings", Logger::LogLevel::WARN, "Ignoring invalid WiFi password length. Expected 8-63 characters.");
	}
}

String Settings::GetDeviceIpAddress() {
	String ip;

	if (!store.IsKey(keyDeviceIP)) {
	    Logger::Log("Settings", Logger::LogLevel::WARN,
	        "Missing key '%s'. Using default '%s'",
	        keyDeviceIP, defaultDeviceIP.c_str());
	} else {
	    ip = store.GetString(keyDeviceIP, defaultDeviceIP);
	    ip.trim();
	    if (ip.isEmpty()) {
	        Logger::Log("Settings", Logger::LogLevel::WARN,
	            "Invalid empty value for '%s'. Using default '%s'",
	            keyDeviceIP, defaultDeviceIP.c_str());
	    }
	}

	const String result = ip.isEmpty() ? defaultDeviceIP : ip;

	if (result == defaultDeviceIP) {
	    store.PutString(keyDeviceIP, defaultDeviceIP);
	}

	return result;
}

void Settings::SetExternalNWE_UseDHCP(bool useDHCP) {
	store.PutInt(keyExternalNW_UseDHCP, (int)useDHCP);
	SetModified();
}

bool Settings::GetExternalNWE_UseDHCP() {
	return (bool)store.GetInt(keyExternalNW_UseDHCP, (int)defaultExternalNW_UseDHCP);
}

String Settings::GetExternalNW_IP() {
	return store.IsKey(keyExternalNW_IP) ? store.GetString(keyExternalNW_IP, defaultExternalNW_IP) : defaultExternalNW_IP;
}

void Settings::SetExternalNW_IP(String ip) {
	store.PutString(keyExternalNW_IP, ip);
	SetModified();
}

String Settings::GetExternalNW_SNM() {
	return store.IsKey(keyExternalNW_SNM) ? store.GetString(keyExternalNW_SNM, defaultExternalNW_SNM) : defaultExternalNW_SNM;
}

void Settings::SetExternalNW_SNM(String snm) {
	store.PutString(keyExternalNW_SNM, snm);
	SetModified();
}

String Settings::GetExternalNW_DFG() {
	return store.IsKey(keyExternalNW_DFG) ? store.GetString(keyExternalNW_DFG, defaultExternalNW_DFG) : defaultExternalNW_DFG;
}

void Settings::SetExternalNW_DFG(String dfg) {
	store.PutString(keyExternalNW_DFG, dfg);
	SetModified();
}

String Settings::GetExternalNW_DNSIP() {
	return store.IsKey(keyExternalNW_DNSIP) ? store.GetString(keyExternalNW_DNSIP, defaultExternalNW_DNSIP) : defaultExternalNW_DNSIP;
}

void Settings::SetExternalNW_DNSIP(String dnsIP) {
	store.PutString(keyExternalNW_DNSIP, dnsIP);
	SetModified();
}

String Settings::GetExternalNW_AuthMode() {
	String authMode = store.IsKey(keyExternalNW_AuthMode)
		? store.GetString(keyExternalNW_AuthMode, defaultExternalNW_AuthMode)
		: defaultExternalNW_AuthMode;

	return NormalizeExternalNetworkAuthMode(authMode);
}

void Settings::SetExternalNW_AuthMode(String authMode) {
	store.PutString(keyExternalNW_AuthMode, NormalizeExternalNetworkAuthMode(authMode));
	SetModified();
}

String Settings::GetExternalNW_EnterpriseIdentity() {
	return store.IsKey(keyExternalNW_EnterpriseIdentity)
		? store.GetString(keyExternalNW_EnterpriseIdentity, defaultExternalNW_EnterpriseIdentity)
		: defaultExternalNW_EnterpriseIdentity;
}

void Settings::SetExternalNW_EnterpriseIdentity(String identity) {
	identity.trim();
	store.PutString(keyExternalNW_EnterpriseIdentity, identity);
	SetModified();
}

String Settings::GetExternalNW_EnterpriseUsername() {
	return store.IsKey(keyExternalNW_EnterpriseUsername)
		? store.GetString(keyExternalNW_EnterpriseUsername, defaultExternalNW_EnterpriseUsername)
		: defaultExternalNW_EnterpriseUsername;
}

void Settings::SetExternalNW_EnterpriseUsername(String username) {
	username.trim();
	store.PutString(keyExternalNW_EnterpriseUsername, username);
	SetModified();
}

String Settings::GetExternalNW_EnterpriseAnonymousIdentity() {
	return store.IsKey(keyExternalNW_EnterpriseAnonymousIdentity)
		? store.GetString(keyExternalNW_EnterpriseAnonymousIdentity, defaultExternalNW_EnterpriseAnonymousIdentity)
		: defaultExternalNW_EnterpriseAnonymousIdentity;
}

void Settings::SetExternalNW_EnterpriseAnonymousIdentity(String anonymousIdentity) {
	anonymousIdentity.trim();
	store.PutString(keyExternalNW_EnterpriseAnonymousIdentity, anonymousIdentity);
	SetModified();
}

String Settings::GetExternalNW_EnterprisePassword() {
	return store.IsKey(keyExternalNW_EnterprisePassword)
		? store.GetString(keyExternalNW_EnterprisePassword, defaultExternalNW_EnterprisePassword)
		: defaultExternalNW_EnterprisePassword;
}

void Settings::SetExternalNW_EnterprisePassword(String password) {
	password.trim();

	if(password.isEmpty())
	{
		if (store.IsKey(keyExternalNW_EnterprisePassword)) {
			store.Remove(keyExternalNW_EnterprisePassword);
			SetModified();
		}
	} else if (password.length() >= 8 && password.length() < 64) {
		store.PutString(keyExternalNW_EnterprisePassword, password);
		SetModified();
	} else {
		Logger::Log("Settings", Logger::LogLevel::WARN, "Ignoring invalid enterprise password length. Expected 8-63 characters.");
	}
}

String Settings::GetExternalNW_EnterprisePhase2Method() {
	String phase2Method = store.IsKey(keyExternalNW_EnterprisePhase2Method)
		? store.GetString(keyExternalNW_EnterprisePhase2Method, defaultExternalNW_EnterprisePhase2Method)
		: defaultExternalNW_EnterprisePhase2Method;

	return NormalizeExternalNetworkPhase2Method(phase2Method);
}

void Settings::SetExternalNW_EnterprisePhase2Method(String phase2Method) {
	store.PutString(keyExternalNW_EnterprisePhase2Method, NormalizeExternalNetworkPhase2Method(phase2Method));
	SetModified();
}

String Settings::GetExternalNW_EnterpriseCaCertificate() {
	return store.IsKey(keyExternalNW_EnterpriseCaCertificate)
		? store.GetString(keyExternalNW_EnterpriseCaCertificate, defaultExternalNW_EnterpriseCaCertificate)
		: defaultExternalNW_EnterpriseCaCertificate;
}

void Settings::SetExternalNW_EnterpriseCaCertificate(String caCertificate) {
	caCertificate.trim();
	store.PutString(keyExternalNW_EnterpriseCaCertificate, caCertificate);
	SetModified();
}

String Settings::GetExternalNW_EnterpriseClientCertificate() {
	return store.IsKey(keyExternalNW_EnterpriseClientCertificate)
		? store.GetString(keyExternalNW_EnterpriseClientCertificate, defaultExternalNW_EnterpriseClientCertificate)
		: defaultExternalNW_EnterpriseClientCertificate;
}

void Settings::SetExternalNW_EnterpriseClientCertificate(String clientCertificate) {
	clientCertificate.trim();
	store.PutString(keyExternalNW_EnterpriseClientCertificate, clientCertificate);
	SetModified();
}

String Settings::GetExternalNW_EnterpriseClientPrivateKey() {
	return store.IsKey(keyExternalNW_EnterpriseClientPrivateKey)
		? store.GetString(keyExternalNW_EnterpriseClientPrivateKey, defaultExternalNW_EnterpriseClientPrivateKey)
		: defaultExternalNW_EnterpriseClientPrivateKey;
}

void Settings::SetExternalNW_EnterpriseClientPrivateKey(String clientPrivateKey) {
	clientPrivateKey.trim();
	store.PutString(keyExternalNW_EnterpriseClientPrivateKey, clientPrivateKey);
	SetModified();
}

void Settings::SetDeviceIpAddress(String ip) {
	ip.trim();
	if (!ip.isEmpty()) {
		IPAddress parsedIp;
		if (!parsedIp.fromString(ip)) {
			Logger::Log("Settings", Logger::LogLevel::WARN, "Ignoring invalid device IP '%s'", ip.c_str());
		} else {
			store.PutString(keyDeviceIP, ip);
			SetModified();
		}
	}
}

String Settings::GetSubnetMask() {
	String mask;

	if (!store.IsKey(keySubnetMask)) {
	    Logger::Log("Settings", Logger::LogLevel::WARN,
	        "Missing key '%s'. Using default '%s'",
	        keySubnetMask, defaultSubnetMask.c_str());
	} else {
	    mask = store.GetString(keySubnetMask, defaultSubnetMask);
	    mask.trim();
	    if (mask.isEmpty()) {
	        Logger::Log("Settings", Logger::LogLevel::WARN,
	            "Invalid empty value for '%s'. Using default '%s'",
	            keySubnetMask, defaultSubnetMask.c_str());
	    }
	}

	const String result = mask.isEmpty() ? defaultSubnetMask : mask;

	if (result == defaultSubnetMask) {
	    store.PutString(keySubnetMask, defaultSubnetMask);
	}

	return result;
}

void Settings::SetSubnetMask(String mask) {
	mask.trim();
	if (!mask.isEmpty()) {
		IPAddress parsedMask;
		if (!parsedMask.fromString(mask)) {
			Logger::Log("Settings", Logger::LogLevel::WARN, "Ignoring invalid subnet mask '%s'", mask.c_str());
		} else {
			store.PutString(keySubnetMask, mask);
			SetModified();
		}
	}
}

bool Settings::AdvancedSettingsEnabled() {
	return store.IsKey(keyEnableAdvancedSettings) ? (bool)store.GetInt(keyEnableAdvancedSettings, defaultEnableAdvancedSettings) : defaultEnableAdvancedSettings;
}

void Settings::SetAdvancedSettingsEnabled(bool enable) {
	store.PutInt(keyEnableAdvancedSettings, (int)enable);
	SetModified();
}

bool Settings::DNSEnabled() {
	return store.IsKey(keyEnableDNS) ? (bool)store.GetInt(keyEnableDNS, defaultEnableDNS) : defaultEnableDNS;
}

void Settings::SetDNSEnabled(bool enable) {
	store.PutInt(keyEnableDNS, (int)enable);
	GoalFinderApp::GetInstance()->SetDNSEnabled(enable);
	SetModified();
}