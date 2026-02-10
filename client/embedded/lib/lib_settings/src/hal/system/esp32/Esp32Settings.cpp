// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma message("ESP32: Settings HAL")

#include <system/Settings.h>
#include <Preferences.h>

namespace System {

struct Settings::SettingsInstanceData {
	Preferences mPfPrefs;
};

Settings::Settings() :
        mInitialized(false), mReadOnly(false), mInstanceData(0) {
			SettingsInstanceData* instanceData = new SettingsInstanceData();
			mInstanceData = instanceData;
}

Settings::~Settings() {
	End();
	delete mInstanceData;
	mInstanceData = 0;
}

bool Settings::Begin(const char* name, bool readOnly, const char* partition_label) {
	return GetInstanceData()->mPfPrefs.begin(name, readOnly, partition_label);
}

void Settings::End() {
	GetInstanceData()->mPfPrefs.end();
	mInitialized = false;
}

bool Settings::Clear() { 
	return GetInstanceData()->mPfPrefs.clear();
}

bool Settings::Remove(const char* key) {
	return GetInstanceData()->mPfPrefs.remove(key);
}

size_t Settings::PutChar(const char* key, int8_t value) {
	return GetInstanceData()->mPfPrefs.putChar(key, value);
}

size_t Settings::PutUChar(const char* key, uint8_t value) {
	return GetInstanceData()->mPfPrefs.putUChar(key, value);
}

size_t Settings::PutShort(const char* key, int16_t value) {
	return GetInstanceData()->mPfPrefs.putShort(key, value);
}

size_t Settings::PutUShort(const char* key, uint16_t value) {
	return GetInstanceData()->mPfPrefs.putUShort(key, value);
}

size_t Settings::PutInt(const char* key, int32_t value) {
	return GetInstanceData()->mPfPrefs.putInt(key, value);
}

size_t Settings::PutUInt(const char* key, uint32_t value) {
	return GetInstanceData()->mPfPrefs.putUInt(key, value);
}

size_t Settings::PutLong(const char* key, int32_t value) {
	return GetInstanceData()->mPfPrefs.putLong(key, value);
}

size_t Settings::PutULong(const char* key, uint32_t value) {
	return GetInstanceData()->mPfPrefs.putULong(key, value);
}

size_t Settings::PutLong64(const char* key, int64_t value) {
	return GetInstanceData()->mPfPrefs.putLong64(key, value);
}

size_t Settings::PutULong64(const char* key, uint64_t value) { 
	return GetInstanceData()->mPfPrefs.putULong64(key, value);
}

size_t Settings::PutFloat(const char* key, const float_t value) {
	return GetInstanceData()->mPfPrefs.putFloat(key, value);
}

size_t Settings::PutDouble(const char* key, const double_t value) {
	return GetInstanceData()->mPfPrefs.putDouble(key, value);
}

size_t Settings::PutBool(const char* key, const bool value) {
	return GetInstanceData()->mPfPrefs.putBool(key, value);
}

size_t Settings::PutString(const char* key, const char* value) {
	return GetInstanceData()->mPfPrefs.putString(key, value);
}

size_t Settings::PutString(const char* key, const String value) {
	return GetInstanceData()->mPfPrefs.putString(key, value);
}

size_t Settings::PutBytes(const char* key, const void* value, size_t len) {
	return GetInstanceData()->mPfPrefs.putBytes(key, value, len);
}

SettingsType Settings::GetType(const char* key) {
	return (SettingsType)GetInstanceData()->mPfPrefs.getType(key);
}

bool Settings::IsKey(const char* key) {
	return GetInstanceData()->mPfPrefs.isKey(key);
}

int8_t Settings::GetChar(const char* key, const int8_t defaultValue) {
	return GetInstanceData()->mPfPrefs.getChar(key, defaultValue);
}

uint8_t Settings::GetUChar(const char* key, const uint8_t defaultValue) {
	return GetInstanceData()->mPfPrefs.getUChar(key, defaultValue);
}

int16_t Settings::GetShort(const char* key, const int16_t defaultValue) {
	return GetInstanceData()->mPfPrefs.getShort(key, defaultValue);
}

uint16_t Settings::GetUShort(const char* key, const uint16_t defaultValue) {
	return GetInstanceData()->mPfPrefs.getUShort(key, defaultValue);
}

int32_t Settings::GetInt(const char* key, const int32_t defaultValue) {
	return GetInstanceData()->mPfPrefs.getInt(key, defaultValue);
}

uint32_t Settings::GetUInt(const char* key, const uint32_t defaultValue) {
	return GetInstanceData()->mPfPrefs.getUInt(key, defaultValue);
}

int32_t Settings::GetLong(const char* key, const int32_t defaultValue) {
	return GetInstanceData()->mPfPrefs.getLong(key, defaultValue);
}

uint32_t Settings::GetULong(const char* key, const uint32_t defaultValue) {
	return GetInstanceData()->mPfPrefs.getULong(key, defaultValue);
}

int64_t Settings::GetLong64(const char* key, const int64_t defaultValue) {
	return GetInstanceData()->mPfPrefs.getLong64(key, defaultValue);
}

uint64_t Settings::GetULong64(const char* key, const uint64_t defaultValue) {
	return GetInstanceData()->mPfPrefs.getULong64(key, defaultValue);
}

float_t Settings::GetFloat(const char* key, const float_t defaultValue) {
	return GetInstanceData()->mPfPrefs.getFloat(key, defaultValue);
}

double_t Settings::GetDouble(const char* key, const double_t defaultValue) {
	return GetInstanceData()->mPfPrefs.getDouble(key, defaultValue);
}

bool Settings::GetBool(const char* key, const bool defaultValue) {
	return GetInstanceData()->mPfPrefs.getBool(key, defaultValue);
}

size_t Settings::GetString(const char* key, char* defaultValue, const size_t maxLen) {
	return GetInstanceData()->mPfPrefs.getString(key, defaultValue, maxLen);
}

String Settings::GetString(const char* key, const String defaultValue) {
	return GetInstanceData()->mPfPrefs.getString(key, defaultValue);
}

size_t Settings::GetBytesLength(const char* key) {
	return GetInstanceData()->mPfPrefs.getBytesLength(key);
}

size_t Settings::GetBytes(const char* key, void* buf, size_t maxLen) {
	return GetInstanceData()->mPfPrefs.getBytes(key, buf, maxLen);
}

size_t Settings::FreeEntries() {
	return GetInstanceData()->mPfPrefs.freeEntries();
}

Settings::SettingsInstanceData* Settings::GetInstanceData() {
	return (SettingsInstanceData*)mInstanceData;
}

} // namespace System
