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

#pragma message("DUMMY: Settings HAL")

#include <system/Settings.h>
#include <utils/StringUtils.h>
#include <utils/Log.h>

namespace System {

struct Settings::SettingsInstanceData {
};

Settings::Settings() :
        mInitialized(false), mReadOnly(false), mInstanceData(0) {
}

Settings::~Settings() {
}

bool Settings::Begin(const char* name, bool readOnly, const char* partition_label) {
	LogDbg("Dummy Settings: Begin(\"%s\", %s, \"%s\")", name, readOnly, partition_label);
	return false;
}

void Settings::End() {
	LogDbg("Dummy Settings: End()");
}

bool Settings::Clear() { 
	LogDbg("Dummy Settings: Clear()");
	return false;
}

bool Settings::Remove(const char* key) {
	LogDbg("Dummy Settings: Remove(\"%s\")", key);
	return false;
}

size_t Settings::PutChar(const char* key, int8_t value) {
	LogDbg("Dummy Settings: PutChar(\"%s\", '%c')", key, value);
	return 0;
}

size_t Settings::PutUChar(const char* key, uint8_t value) {
	LogDbg("Dummy Settings: PutUChar(\"%s\", '%c')", key, value);
	return 0;
}

size_t Settings::PutShort(const char* key, int16_t value) {
	LogDbg("Dummy Settings: PutShort(\"%s\", %d)", key, value);
	return 0;
}

size_t Settings::PutUShort(const char* key, uint16_t value) {
	LogDbg("Dummy Settings: PutUShort(\"%s\", %d)", key, value);
	return 0;
}

size_t Settings::PutInt(const char* key, int32_t value) {
	LogDbg("Dummy Settings: PutInt(\"%s\", %d)", key, value);
	return 0;
}

size_t Settings::PutUInt(const char* key, uint32_t value) {
	LogDbg("Dummy Settings: PutUInt(\"%s\", %d)", key, value);
	return 0;
}

size_t Settings::PutLong(const char* key, int32_t value) {
	LogDbg("Dummy Settings: PutLong(\"%s\", %d)", key, value);
	return 0;
}

size_t Settings::PutULong(const char* key, uint32_t value) {
	LogDbg("Dummy Settings: PutULong(\"%s\", %d)", key, value);
	return 0;
}

size_t Settings::PutLong64(const char* key, int64_t value) {
	LogDbg("Dummy Settings: PutLong64(\"%s\", %d)", key, value);
	return 0;
}

size_t Settings::PutULong64(const char* key, uint64_t value) { 
	LogDbg("Dummy Settings: PutULong64(\"%s\", %d)", key, value);
	return 0;
}

size_t Settings::PutFloat(const char* key, const float_t value) {
	LogDbg("Dummy Settings: PutFloat(\"%s\", %d)", key, value);
	return 0;
}

size_t Settings::PutDouble(const char* key, const double_t value) {
	LogDbg("Dummy Settings: PutDouble(\"%s\", %d)", key, value);
	return 0;
}

size_t Settings::PutBool(const char* key, const bool value) {
	LogDbg("Dummy Settings: PutBool(\"%s\", %s)", key, value ? "true" : "false");
	return 0;
}

size_t Settings::PutString(const char* key, const char* value) {
	LogDbg("Dummy Settings: PutString(\"%s\", %d)", key, value);
	return 0;
}

size_t Settings::PutString(const char* key, const String value) {
	LogDbg("Dummy Settings: PutString(\"%s\", %d)", key, value);
	return 0;
}

size_t Settings::PutBytes(const char* key, const void* value, size_t len) {
	LogDbg("Dummy Settings: PutBytes(\"%s\", %d)", key, value);
	return 0;
}

SettingsType Settings::GetType(const char* key) {
	LogDbg("Dummy Settings: GetType(\"%s\")", key);
	return PT_INVALID;
}

bool Settings::IsKey(const char* key) {
	LogDbg("Dummy Settings: IsKey(\"%s\")", key);
	return false;
}

int8_t Settings::GetChar(const char* key, const int8_t defaultValue) {
	LogDbg("Dummy Settings: GetChar(\"%s\", '%c')", key, defaultValue);
	return '0';
}

uint8_t Settings::GetUChar(const char* key, const uint8_t defaultValue) {
	LogDbg("Dummy Settings: GetChar(\"%s\", '%c')", key, defaultValue);
	return '0';
}

int16_t Settings::GetShort(const char* key, const int16_t defaultValue) {
	LogDbg("Dummy Settings: GetShort(\"%s\", %d)", key, defaultValue);
	return 0;
}

uint16_t Settings::GetUShort(const char* key, const uint16_t defaultValue) {
	LogDbg("Dummy Settings: GetUShort(\"%s\", %d)", key, defaultValue);
	return 0;
}

int32_t Settings::GetInt(const char* key, const int32_t defaultValue) {
	LogDbg("Dummy Settings: GetInt(\"%s\", %d)", key, defaultValue);
	return 0;
}

uint32_t Settings::GetUInt(const char* key, const uint32_t defaultValue) {
	LogDbg("Dummy Settings: GetUInt(\"%s\", %d)", key, defaultValue);
	return 0;
}

int32_t Settings::GetLong(const char* key, const int32_t defaultValue) {
	LogDbg("Dummy Settings: GetLong(\"%s\", %d)", key, defaultValue);
	return 0;
}

uint32_t Settings::GetULong(const char* key, const uint32_t defaultValue) {
	LogDbg("Dummy Settings: GetULong(\"%s\", %d)", key, defaultValue);
	return 0;
}

int64_t Settings::GetLong64(const char* key, const int64_t defaultValue) {
	LogDbg("Dummy Settings: GetLong64(\"%s\", %ld)", key, defaultValue);
	return 0;
}

uint64_t Settings::GetULong64(const char* key, const uint64_t defaultValue) {
	LogDbg("Dummy Settings: GetULong64(\"%s\", %ld)", key, defaultValue);
	return 0;
}

float_t Settings::GetFloat(const char* key, const float_t defaultValue) {
	LogDbg("Dummy Settings: GetFloat(\"%s\", %f)", key, defaultValue);
	return 0.0;
}

double_t Settings::GetDouble(const char* key, const double_t defaultValue) {
	LogDbg("Dummy Settings: GetDouble(\"%s\", %f)", key, defaultValue);
	return 0.0;
}

bool Settings::GetBool(const char* key, const bool defaultValue) {
	LogDbg("Dummy Settings: GetBool(\"%s\", %s)", key, defaultValue ? "true" : "false");
	return false;
}

size_t Settings::GetString(const char* key, char* value, const size_t maxLen) {
	LogDbg("Dummy Settings: GetString(\"%s\", %s, %d)", key, value, maxLen);
	return 0;
}

String Settings::GetString(const char* key, const String defaultValue) {
	LogDbg("Dummy Settings: GetString(\"%s\", %s, %d)", key, defaultValue);
	return "no data";
}

size_t Settings::GetBytesLength(const char* key) {
	LogDbg("Dummy Settings: GetBytesLength(\"%s\")", key);
	return 0;
}

size_t Settings::GetBytes(const char* key, void* buf, size_t maxLen) {
	LogDbg("Dummy Settings: getBytes(\"%s\", %p, %d)", key, buf, maxLen);
	return 0;
}

size_t Settings::FreeEntries() {
	LogDbg("Dummy Settings: FreeEntries()");
	return 0;
}

Settings::SettingsInstanceData* Settings::GetInstanceData() {
	return 0;
}

} // namespace System
