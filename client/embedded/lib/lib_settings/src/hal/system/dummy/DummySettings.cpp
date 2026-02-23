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
#include "util/Logger.h"

namespace System {

struct Settings::SettingsInstanceData {
};

Settings::Settings() :
        mInitialized(false), mReadOnly(false), mInstanceData(0) {
}

Settings::~Settings() {
}

bool Settings::Begin(const char* name, bool readOnly, const char* partition_label) {
	Logger::log("Settings", Logger::LogLevel::DEBUG, "Dummy Settings: Begin(\"%s\", %d, \"%s\")", name, readOnly, partition_label);
	return false;
}

void Settings::End() {
	Logger::log("Settings", Logger::LogLevel::DEBUG, "Dummy Settings: End()");
}

bool Settings::Clear() { 
	Logger::log("Settings", Logger::LogLevel::DEBUG, "Dummy Settings: Clear()");
	return false;
}

bool Settings::Remove(const char* key) {
	Logger::log("Settings", Logger::LogLevel::DEBUG, "Dummy Settings: Remove(\"%s\")", key);
	return false;
}

size_t Settings::PutChar(const char* key, int8_t value) {
	Logger::log("Settings", Logger::LogLevel::DEBUG, "Dummy Settings: PutChar(\"%s\", '%c')", key, value);
	return 0;
}

size_t Settings::PutUChar(const char* key, uint8_t value) {
	Logger::log("Settings", Logger::LogLevel::DEBUG, "Dummy Settings: PutUChar(\"%s\", '%c')", key, value);
	return 0;
}

size_t Settings::PutShort(const char* key, int16_t value) {
	Logger::log("Settings", Logger::LogLevel::DEBUG, "Dummy Settings: PutShort(\"%s\", %d)", key, value);
	return 0;
}

size_t Settings::PutUShort(const char* key, uint16_t value) {
	Logger::log("Settings", Logger::LogLevel::DEBUG, "Dummy Settings: PutUShort(\"%s\", %d)", key, value);
	return 0;
}

size_t Settings::PutInt(const char* key, int32_t value) {
	Logger::log("Settings", Logger::LogLevel::DEBUG, "Dummy Settings: PutInt(\"%s\", %d)", key, value);
	return 0;
}

size_t Settings::PutUInt(const char* key, uint32_t value) {
	Logger::log("Settings", Logger::LogLevel::DEBUG, "Dummy Settings: PutUInt(\"%s\", %d)", key, value);
	return 0;
}

size_t Settings::PutLong(const char* key, int32_t value) {
	Logger::log("Settings", Logger::LogLevel::DEBUG, "Dummy Settings: PutLong(\"%s\", %d)", key, value);
	return 0;
}

size_t Settings::PutULong(const char* key, uint32_t value) {
	Logger::log("Settings", Logger::LogLevel::DEBUG, "Dummy Settings: PutULong(\"%s\", %d)", key, value);
	return 0;
}

size_t Settings::PutLong64(const char* key, int64_t value) {
	Logger::log("Settings", Logger::LogLevel::DEBUG, "Dummy Settings: PutLong64(\"%s\", %lld)", key, value);
	return 0;
}

size_t Settings::PutULong64(const char* key, uint64_t value) { 
	Logger::log("Settings", Logger::LogLevel::DEBUG, "Dummy Settings: PutULong64(\"%s\", %llu)", key, value);
	return 0;
}

size_t Settings::PutFloat(const char* key, const float_t value) {
	Logger::log("Settings", Logger::LogLevel::DEBUG, "Dummy Settings: PutFloat(\"%s\", %f)", key, value);
	return 0;
}

size_t Settings::PutDouble(const char* key, const double_t value) {
	Logger::log("Settings", Logger::LogLevel::DEBUG, "Dummy Settings: PutDouble(\"%s\", %f)", key, value);
	return 0;
}

size_t Settings::PutBool(const char* key, const bool value) {
	Logger::log("Settings", Logger::LogLevel::DEBUG, "Dummy Settings: PutBool(\"%s\", %s)", key, value ? "true" : "false");
	return 0;
}

size_t Settings::PutString(const char* key, const char* value) {
	Logger::log("Settings", Logger::LogLevel::DEBUG, "Dummy Settings: PutString(\"%s\", %s)", key, value);
	return 0;
}

size_t Settings::PutString(const char* key, const String value) {
	Logger::log("Settings", Logger::LogLevel::DEBUG, "Dummy Settings: PutString(\"%s\", %s)", key, value.c_str());
	return 0;
}

size_t Settings::PutBytes(const char* key, const void* value, size_t len) {
	Logger::log("Settings", Logger::LogLevel::DEBUG, "Dummy Settings: PutBytes(\"%s\", %p)", key, value);
	return 0;
}

SettingsType Settings::GetType(const char* key) {
	Logger::log("Settings", Logger::LogLevel::DEBUG, "Dummy Settings: GetType(\"%s\")", key);
	return PT_INVALID;
}

bool Settings::IsKey(const char* key) {
	Logger::log("Settings", Logger::LogLevel::DEBUG, "Dummy Settings: IsKey(\"%s\")", key);
	return false;
}

int8_t Settings::GetChar(const char* key, const int8_t defaultValue) {
	Logger::log("Settings", Logger::LogLevel::DEBUG, "Dummy Settings: GetChar(\"%s\", '%c')", key, defaultValue);
	return '0';
}

uint8_t Settings::GetUChar(const char* key, const uint8_t defaultValue) {
	Logger::log("Settings", Logger::LogLevel::DEBUG, "Dummy Settings: GetChar(\"%s\", '%c')", key, defaultValue);
	return '0';
}

int16_t Settings::GetShort(const char* key, const int16_t defaultValue) {
	Logger::log("Settings", Logger::LogLevel::DEBUG, "Dummy Settings: GetShort(\"%s\", %d)", key, defaultValue);
	return 0;
}

uint16_t Settings::GetUShort(const char* key, const uint16_t defaultValue) {
	Logger::log("Settings", Logger::LogLevel::DEBUG, "Dummy Settings: GetUShort(\"%s\", %d)", key, defaultValue);
	return 0;
}

int32_t Settings::GetInt(const char* key, const int32_t defaultValue) {
	Logger::log("Settings", Logger::LogLevel::DEBUG, "Dummy Settings: GetInt(\"%s\", %d)", key, defaultValue);
	return 0;
}

uint32_t Settings::GetUInt(const char* key, const uint32_t defaultValue) {
	Logger::log("Settings", Logger::LogLevel::DEBUG, "Dummy Settings: GetUInt(\"%s\", %d)", key, defaultValue);
	return 0;
}

int32_t Settings::GetLong(const char* key, const int32_t defaultValue) {
	Logger::log("Settings", Logger::LogLevel::DEBUG, "Dummy Settings: GetLong(\"%s\", %d)", key, defaultValue);
	return 0;
}

uint32_t Settings::GetULong(const char* key, const uint32_t defaultValue) {
	Logger::log("Settings", Logger::LogLevel::DEBUG, "Dummy Settings: GetULong(\"%s\", %d)", key, defaultValue);
	return 0;
}

int64_t Settings::GetLong64(const char* key, const int64_t defaultValue) {
	Logger::log("Settings", Logger::LogLevel::DEBUG, "Dummy Settings: GetLong64(\"%s\", %lld)", key, defaultValue);
	return 0;
}

uint64_t Settings::GetULong64(const char* key, const uint64_t defaultValue) {
	Logger::log("Settings", Logger::LogLevel::DEBUG, "Dummy Settings: GetULong64(\"%s\", %llu)", key, defaultValue);
	return 0;
}

float_t Settings::GetFloat(const char* key, const float_t defaultValue) {
	Logger::log("Settings", Logger::LogLevel::DEBUG, "Dummy Settings: GetFloat(\"%s\", %f)", key, defaultValue);
	return 0.0;
}

double_t Settings::GetDouble(const char* key, const double_t defaultValue) {
	Logger::log("Settings", Logger::LogLevel::DEBUG, "Dummy Settings: GetDouble(\"%s\", %f)", key, defaultValue);
	return 0.0;
}

bool Settings::GetBool(const char* key, const bool defaultValue) {
	Logger::log("Settings", Logger::LogLevel::DEBUG, "Dummy Settings: GetBool(\"%s\", %s)", key, defaultValue ? "true" : "false");
	return false;
}

size_t Settings::GetString(const char* key, char* value, const size_t maxLen) {
	Logger::log("Settings", Logger::LogLevel::DEBUG, "Dummy Settings: GetString(\"%s\", %p, %d)", key, value, maxLen);
	return 0;
}

String Settings::GetString(const char* key, const String defaultValue) {
	Logger::log("Settings", Logger::LogLevel::DEBUG, "Dummy Settings: GetString(\"%s\", %s)", key, defaultValue.c_str());
	return "no data";
}

size_t Settings::GetBytesLength(const char* key) {
	Logger::log("Settings", Logger::LogLevel::DEBUG, "Dummy Settings: GetBytesLength(\"%s\")", key);
	return 0;
}

size_t Settings::GetBytes(const char* key, void* buf, size_t maxLen) {
	Logger::log("Settings", Logger::LogLevel::DEBUG, "Dummy Settings: getBytes(\"%s\", %p, %d)", key, buf, maxLen);
	return 0;
}

size_t Settings::FreeEntries() {
	Logger::log("Settings", Logger::LogLevel::DEBUG, "Dummy Settings: FreeEntries()");
	return 0;
}

Settings::SettingsInstanceData* Settings::GetInstanceData() {
	return 0;
}

} // namespace System
