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
#include "util/Logger.h"

const char* Settings::keyVolume = "volume";
const int Settings::defaultVolume = 25;

const char* Settings::keyMetronomeSound = "metronomeSound";
const int Settings::defaultMetronomeSound = 0;

const char* Settings::keyHitSound = "hitSound";
const int Settings::defaultHitSound = 0;

const char* Settings::keyMissSound = "missSound";
const int Settings::defaultMissSound = 0;

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

const char* Settings::keyDeviceIpAddress = "deviceIP";
const String Settings::defaultDeviceIpAddress = "192.168.4.1";

const char* Settings::keySubnetMask = "subnetMask";
const String Settings::defaultSubnetMask = "255.255.255.0";
	
Settings::Settings() :
    Singleton<Settings>(),
	store(),
	modified(false)
	{
    	store.Begin("app_prefs");
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
	metronomeSound = max(min(metronomeSound, 2), 0);
	store.PutInt(keyMetronomeSound, metronomeSound);
	SetModified();
}

int Settings::GetMetronomeSound() {
	return store.GetInt(keyMetronomeSound, defaultMetronomeSound);
}

int Settings::GetHitSound() {
	return store.GetInt(keyHitSound, defaultHitSound);
}

void Settings::SetHitSound(int hitSound) {
	hitSound = max(min(hitSound, 2), 0);
	store.PutInt(keyHitSound, hitSound);
	SetModified();
}

void Settings::SetMissSound(int missSound) {
	missSound = max(min(missSound, 2), 0);
	store.PutInt(keyMissSound, missSound);
	SetModified();
}

int Settings::GetMissSound() {
	return store.GetInt(keyMissSound, defaultMissSound);
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

String Settings::GetDeviceName()
{
	return store.GetString(keyDeviceName, defaultDeviceName);
}

void Settings::SetDeviceName(String deviceName)
{
	if(deviceName.isEmpty())
	{
		deviceName = defaultDeviceName;
	}
	
	store.PutString(keyDeviceName, deviceName);
	SetModified(); 
}

String Settings::GetDevicePassword()
{
	return store.IsKey(keyDevicePassword) ? store.GetString(keyDevicePassword, defaultDevicePassword) : defaultDevicePassword;
}

void Settings::SetDevicePassword(String devicePassword)
{
	if(devicePassword.isEmpty())
	{
		if (store.IsKey(keyDevicePassword)) {
			store.Remove(keyDevicePassword);
		}
	}
	else 
	{
		store.PutString(keyDevicePassword, devicePassword);
	}
	SetModified();
}

String Settings::GetWifiPassword()
{
	return store.IsKey(keyWifiPassword) ? store.GetString(keyWifiPassword, defaultWifiPassword) : defaultWifiPassword;
}

void Settings::SetWifiPassword(String wifiPassword)
{
	wifiPassword.trim();

	if(wifiPassword.isEmpty())
	{
		if (store.IsKey(keyWifiPassword)) {
			store.Remove(keyWifiPassword);
			SetModified();
		}
	} else if (wifiPassword.length() >= 8 && wifiPassword.length() < 63) {
		store.PutString(keyWifiPassword, wifiPassword);
		SetModified();		
	} else {
		Logger::log("Settings", Logger::LogLevel::WARN, "Ignoring invalid WiFi password length. Expected 8-63 characters.");
	}
}

int Settings::GetVibrationSensorSensitivity()
{
	return store.GetInt(keyVibrationSensorSensitivity, defaultVibrationSensorSensitivity);
}

void Settings::SetVibrationSensorSensitivity(int vibrationSensorSensitivity)
{
	vibrationSensorSensitivity = max(min(vibrationSensorSensitivity, 100), 0);
	store.PutInt(keyVibrationSensorSensitivity, vibrationSensorSensitivity);
	SetModified();
};

int Settings::GetBallHitDetectionDistance() 
{
	return store.GetInt(keyBallHitDetectionDistance, defaultBallHitDetectionDistance);
}

void Settings::SetBallHitDetectionDistance(int ballHitDetectionDistance)
{
	ballHitDetectionDistance = max(min(ballHitDetectionDistance, 600), 100);
	store.PutInt(keyBallHitDetectionDistance, ballHitDetectionDistance);
	SetModified();
}

bool Settings::GetDistanceOnlyHitDetection()
{
	return (bool)store.GetInt(keyDistanceOnlyHitDetection, (int)defaultDistanceOnlyHitDetection);
}

void Settings::SetDistanceOnlyHitDetection(bool distanceOnlyHitDetection)
{
	store.PutInt(keyDistanceOnlyHitDetection, (int)distanceOnlyHitDetection);
	SetModified();
}

int Settings::GetLedBrightness()
{
	return store.GetInt(keyLedBrightness, defaultLedBrightness);
}

void Settings::SetLedBrightness(int ledBrightness)
{
	ledBrightness = max(min(ledBrightness, 100), 0);
	store.PutInt(keyLedBrightness, ledBrightness);
	SetModified();
}

LedMode Settings::GetLedMode()
{
	return (LedMode)store.GetInt(keyLedMode, (int)defaultLedMode);
};

void Settings::SetLedMode(LedMode ledMode)
{
	store.PutInt(keyLedMode, (int)ledMode);
	SetModified();
};

bool Settings::IsFirstRun()
{
	return (bool)store.GetInt(keyFirstRun, (int)defaultFirstRun);
}

void Settings::SetFirstRun(bool firstRun)
{
	store.PutInt(keyFirstRun, (int)firstRun);
	SetModified();
}
  
void Settings::ResetToDefaults()
{
	store.Clear();
	ESP.restart();
}

int Settings::GetAfterHitTimeout()
{
	return store.GetInt(keyAfterHitTimeout, defaultAfterHitTimeout);
}

void Settings::SetAfterHitTimeout(int timeout) 
{
	timeout = max(min(timeout, 60), 0);
	store.PutInt(keyAfterHitTimeout, timeout);
	SetModified();
}

bool Settings::GetUpdateSuccess()
{
	return (bool)store.GetInt(keyUpdateSuccess, (int)defaultUpdateSuccess);
}

void Settings::SetUpdateSuccess(bool success)
{
	store.PutInt(keyUpdateSuccess, (int)success);
}

bool Settings::GetExtraLog()
{
	return (bool)store.GetInt(keyExtraLog, (int)defaultExtraLog);
}

void Settings::SetExtraLog(bool enabled)
{
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
		Logger::log("Settings", Logger::LogLevel::WARN, "Ignoring invalid WiFi password length. Expected 8-63 characters.");
	}
}

String Settings::GetDeviceIpAddress() {
	return store.GetString(keyDeviceIpAddress, defaultDeviceIpAddress);
}

void Settings::SetDeviceIpAddress(String ip) {
	ip.trim();
	if (!ip.isEmpty()) {
		store.PutString(keyDeviceIpAddress, ip);
		SetModified();
	}
}

String Settings::GetSubnetMask() {
	return store.GetString(keySubnetMask, defaultSubnetMask);
}

void Settings::SetSubnetMask(String mask) {
	mask.trim();
	if (!mask.isEmpty()) {
		store.PutString(keySubnetMask, mask);
		SetModified();
	}
}
