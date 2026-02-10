/*
 * ===============================================================================
 * (c) HTL Leonding
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Licensed under MIT License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * All trademarks used in this document are property of their respective owners.
 * ===============================================================================
 */

/* 
 * This library is interface compatible with Espressif ESP32 Preferences library.
 * provided for ESP32 under the Apache License, Version 2.0 (the "License");
 */

#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <functional>

// #include <FS.h>
// #include <ArduinoJson.h>
#include "WString.h"

namespace System {

typedef enum {
	PT_I8, PT_U8, PT_I16, PT_U16, PT_I32, PT_U32, PT_I64, PT_U64, PT_STR, PT_BLOB, PT_BOOL, PT_FLOAT, PT_DOUBLE, PT_INVALID
} SettingsType;

class Settings {
	struct SettingsInstanceData;

	public:
		Settings();
		~Settings();

		bool Begin(const char* name, bool readOnly = false, const char* partition_label = NULL);
		void End();

		bool Clear();
		bool Remove(const char* key);

		size_t PutChar(const char* key, int8_t value);
		size_t PutUChar(const char* key, uint8_t value);
		size_t PutShort(const char* key, int16_t value);
		size_t PutUShort(const char* key, uint16_t value);
		size_t PutInt(const char* key, int32_t value);
		size_t PutUInt(const char* key, uint32_t value);
		size_t PutLong(const char* key, int32_t value);
		size_t PutULong(const char* key, uint32_t value);
		size_t PutLong64(const char* key, int64_t value);
		size_t PutULong64(const char* key, uint64_t value);
		size_t PutFloat(const char* key, float_t value);
		size_t PutDouble(const char* key, double_t value);
		size_t PutBool(const char* key, bool value);
		size_t PutString(const char* key, const char* value);
		size_t PutString(const char* key, String value);
		size_t PutBytes(const char* key, const void* value, size_t len);

		bool IsKey(const char* key);
		SettingsType GetType(const char* key);
		int8_t GetChar(const char* key, int8_t defaultValue = 0);
		uint8_t GetUChar(const char* key, uint8_t defaultValue = 0);
		int16_t GetShort(const char* key, int16_t defaultValue = 0);
		uint16_t GetUShort(const char* key, uint16_t defaultValue = 0);
		int32_t GetInt(const char* key, int32_t defaultValue = 0);
		uint32_t GetUInt(const char* key, uint32_t defaultValue = 0);
		int32_t GetLong(const char* key, int32_t defaultValue = 0);
		uint32_t GetULong(const char* key, uint32_t defaultValue = 0);
		int64_t GetLong64(const char* key, int64_t defaultValue = 0);
		uint64_t GetULong64(const char* key, uint64_t defaultValue = 0);
		float_t GetFloat(const char* key, float_t defaultValue = NAN);
		double_t GetDouble(const char* key, double_t defaultValue = NAN);
		bool GetBool(const char* key, bool defaultValue = false);
		size_t GetString(const char* key, char* value, size_t maxLen);
		String GetString(const char* key, String defaultValue = String());
		size_t GetBytesLength(const char* key);
		size_t GetBytes(const char* key, void* buf, size_t maxLen);
		size_t FreeEntries();

	private:
		SettingsInstanceData* GetInstanceData();

		bool mInitialized;
		bool mReadOnly;

		SettingsInstanceData* mInstanceData;
};

} // namespace System