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

const char* Settings::keyLedMode = "ledMode";
const LedMode Settings::defaultLedMode = LedMode::Flash;

const char* Settings::keyFirstRun = "firstRun";
const bool Settings::defaultFirstRun = true;
	
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
	hitSound = max(min(hitSound, 0), 0);
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
};

void Settings::SetDeviceName(String deviceName)
{
	if(deviceName.isEmpty())
	{
		deviceName = defaultDeviceName;
		store.Remove(keyDeviceName);
	}
	
	store.PutString(keyDeviceName, deviceName);
	SetModified(); 
};

String Settings::GetDevicePassword()
{
	return store.GetString(keyDevicePassword, defaultDevicePassword);
};

void Settings::SetDevicePassword(String devicePassword)
{
	if(devicePassword.isEmpty())
	{
		devicePassword = emptyString;
		store.Remove(keyDevicePassword);
	}

	store.PutString(keyDevicePassword, devicePassword);
	SetModified();
};

String Settings::GetWifiPassword()
{
	return store.GetString(keyWifiPassword, defaultWifiPassword);
};

void Settings::SetWifiPassword(String wifiPassword)
{
	if(wifiPassword.isEmpty())
	{
		wifiPassword = emptyString;
		store.Remove(keyWifiPassword);
	}

	store.PutString(keyWifiPassword, wifiPassword);
	SetModified();
};

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
	ballHitDetectionDistance = max(min(ballHitDetectionDistance, 200), 0);
	store.PutInt(keyBallHitDetectionDistance, ballHitDetectionDistance);
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