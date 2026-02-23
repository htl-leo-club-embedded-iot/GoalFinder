
// #ifdef ESP8266
#pragma message("ESP8266: Preferences HAL")

#include <rfs/RootFileSystem.h>
#include <utils/StringUtils.h>
#include "util/Logger.h"

namespace System {
// using namespace Rfs;


struct Settings::SettingsInstanceData {
	const char* mNvsPath = "/nvs";
	const char* mDefaultFileName = "unnamed";
	const char* mFileNameExtension = ".pref";

	// File mFile;
	JsonDocument  mDoc(512);

	bool PutValue(const char* key, SettingsType type, ::std::function<void(JsonVariant& valueNode)> valueSupplier) {
		bool rc = false;
		if (mInitialized && key != 0 && !mReadOnly && mFile) {
			JsonObject valObj = mDoc[key];
			if (valObj == 0) {
				valObj = mDoc.createNestedObject(key);
			}
			valObj["t"] = type;
			JsonVariant val = valObj["v"].to<JsonVariant>();
			valueSupplier(val);
			rc = (serializeJson(mDoc, mFile) > 0);
			mFile.flush();
		}
		return rc;
	}

	JsonVariant GetValue(const char* key, SettingsType type) {
		JsonVariant value;
		if (mInitialized && key != 0) {
			JsonObject valObj = mDoc[key];
			if (valObj != 0 && (SettingsType)valObj["t"] == type) {
				value = valObj["v"].as<JsonVariant>();
			}
		}
		return value;
	}

	size_t GetBlob(const char* key, void* buf, size_t maxLen, bool asCStr) {
		size_t len = 0;
		JsonVariant value = GetValue(key, SettingsType::PT_STR);
		if (value.is<String>()) {
			String strVal = value.as<String>();
			if (strVal.length() <= maxLen) {
				len = strVal.length();
				memcpy(buf, strVal.c_str(), len + (asCStr ? 1 : 0));
			}
		}
		return len;
	}
};

Settings::Settings() :
        mInitialized(false), mReadOnly(false), mInstanceData(0) {
			SettingsInstanceData* instanceData = new SettingsInstanceData();
			mInstanceData = instanceData;
}

Settings::~Settings() {
	End();
}

bool Settings::Begin(const char* name, bool readOnly, const char* partition_label) { 
	(void)partition_label; // partition_label is not used by this implementation
	bool rc = mInitialized;
	if (!mInitialized) {
		mReadOnly = readOnly;
		RootFileSystem* rfs = RootFileSystem::GetInstance();
		if (rfs != 0) {
			const char* filenNameCStr = name != 0 ? name : mDefaultFileName;
			Logger::log("Settings", Logger::LogLevel::DEBUG, "pref. file: '%s'", filenNameCStr);
			String prefFilePath(mNvsPath);
			prefFilePath.reserve(1 + strlen(filenNameCStr) + strlen(mFileNameExtension) + 1);
			prefFilePath.concat('/');
			prefFilePath.concat(filenNameCStr);
			prefFilePath.concat(mFileNameExtension);
			Logger::log("Settings", Logger::LogLevel::DEBUG, "Opening preferences file '%s', RO: %c", prefFilePath.c_str(), readOnly ? 'Y' : 'N');
			if (rfs->Exists(mNvsPath) || rfs->MkDir(mNvsPath)) {
				mFile = rfs->Open(prefFilePath,
				        (mReadOnly ? RootFileSystem::FileMode::Read : RootFileSystem::FileMode::WriteTruncate));
				rc = (bool)mFile;
				if (!rc) Logger::log("Settings", Logger::LogLevel::ERROR, "Failed opening preferences file '%s'.", prefFilePath.c_str());
				if (rc) {
					size_t bufferLen = mFile.size() + 1;
					Logger::log("Settings", Logger::LogLevel::DEBUG, "Opened preferences file '%s', %d B", prefFilePath.c_str(), mFile.size());
					uint8_t buffer[bufferLen];
					size_t readLen = 0;
					if (mFile.size() > 0) {
						mFile.read(buffer, bufferLen);
						buffer[readLen] = '\0';
						Logger::log("Settings", Logger::LogLevel::DEBUG, "Deserializing JSON: '%s'", buffer);
						DeserializationError res = deserializeJson(mDoc, buffer);
						Logger::log("Settings", Logger::LogLevel::DEBUG, "Deserialized JSON");
						rc = (res == DeserializationError::Ok);
						if (!rc) Logger::log("Settings", Logger::LogLevel::ERROR, "Failed reading preferences from file '%s': %s", prefFilePath.c_str(), res.c_str());
					}
				}
				if (mReadOnly && mFile) {
					// the file can be closed immediately if preferences cannot be written
					Logger::log("Settings", Logger::LogLevel::DEBUG, "closing file");
					mFile.close();
				}
			} else {
				Logger::log("Settings", Logger::LogLevel::ERROR, "Failed accessing NVS directory '%s'", mNvsPath);
			}
		} else {
			Logger::log("Settings", Logger::LogLevel::ERROR, "Failed accessing RFS");
		}
		mInitialized = rc;
	}
	return rc;
}

void Settings::End() {
	if (mFile) {
		mFile.close();
	}
	mInitialized = false;
}

/* Clear all keys in opened preferences */
bool Settings::Clear() {
	bool rc = !mReadOnly && mFile;
	if (rc) {
		mDoc.clear();
		serializeJson(mDoc, mFile);
	}
	return rc;
}

/* Remove a key */
bool Settings::Remove(const char* key) {
	bool rc = key != 0 && !mReadOnly && mFile;
	if (rc) {
		mDoc.remove(key);
	}
	return rc;
}

/* Put a key value */
size_t Settings::PutChar(const char* key, int8_t value) {
	return GetInstanceData()->PutValue(key, SettingsType::PT_I8, [value](JsonVariant& valueNode) {
		valueNode.set(value);
	}) ? 1 : 0;
}

size_t Settings::PutUChar(const char* key, uint8_t value) {
	return GetInstanceData()->PutValue(key, SettingsType::PT_U8, [value](JsonVariant& valueNode) {
		valueNode.set(value);
	}) ? 1 : 0;
}

size_t Settings::PutShort(const char* key, int16_t value) {
	return GetInstanceData()->PutValue(key, SettingsType::PT_I16, [value](JsonVariant& valueNode) {
		valueNode.set(value);
	}) ? 2 : 0;
}

size_t Settings::PutUShort(const char* key, uint16_t value) {
	return GetInstanceData()->PutValue(key, SettingsType::PT_U16, [value](JsonVariant& valueNode) {
		valueNode.set(value);
	}) ? 2 : 0;
}

size_t Settings::PutInt(const char* key, int32_t value) {
	return GetInstanceData()->PutValue(key, SettingsType::PT_I32, [value](JsonVariant& valueNode) {
		valueNode.set(value);
	}) ? 4 : 0;
}

size_t Settings::PutUInt(const char* key, uint32_t value) {
	return GetInstanceData()->PutValue(key, SettingsType::PT_U32, [value](JsonVariant& valueNode) {
		valueNode.set(value);
	}) ? 4 : 0;
}

size_t Settings::PutLong(const char* key, int32_t value) {
	return PutInt(key, value);
}

size_t Settings::PutULong(const char* key, uint32_t value) {
	return PutUInt(key, value);
}

size_t Settings::PutLong64(const char* key, int64_t value) {
	return GetInstanceData()->PutValue(key, SettingsType::PT_I64, [value](JsonVariant& valueNode) {
		valueNode.set(value);
	}) ? 8 : 0;
}

size_t Settings::PutULong64(const char* key, uint64_t value) {
	return GetInstanceData()->PutValue(key, SettingsType::PT_U64, [value](JsonVariant& valueNode) {
		valueNode.set(value);
	}) ? 8 : 0;
}

size_t Settings::PutFloat(const char* key, const float_t value) {
	return GetInstanceData()->PutValue(key, SettingsType::PT_FLOAT, [value](JsonVariant& valueNode) {
		valueNode.set(value);
	}) ? sizeof(float_t) : 0;
}

size_t Settings::PutDouble(const char* key, const double_t value) {
	return GetInstanceData()->PutValue(key, SettingsType::PT_DOUBLE, [value](JsonVariant& valueNode) {
		valueNode.set(value);
	}) ? sizeof(double_t) : 0;
}

size_t Settings::PutBool(const char* key, const bool value) {
	return GetInstanceData()->PutValue(key, SettingsType::PT_BOOL, [value](JsonVariant& valueNode) {
		valueNode.set(value);
	}) ? sizeof(bool) : 0;
}

size_t Settings::PutString(const char* key, const char* value) {
	return GetInstanceData()->PutValue(key, SettingsType::PT_STR, [value](JsonVariant& valueNode) {
		valueNode.set(value);
	}) ? strlen(value) : 0;
}

size_t Settings::PutString(const char* key, const String value) {
	return PutString(key, value.c_str());
}

size_t Settings::PutBytes(const char* key, const void* value, size_t len) {
	return GetInstanceData()->PutValue(key, SettingsType::PT_BLOB, [value, len](JsonVariant& valueNode) {
		String byteStr = StringUtils::Of((const uint8_t*)value, len);
		valueNode.set(byteStr.c_str());
	}) ? len : 0;
}

SettingsType Settings::GetType(const char* key) {
	SettingsType type = SettingsType::PT_INVALID;
	if (mInitialized && key != 0) {
		JsonObject valObj = mDoc[key];
		if (valObj != 0) {
			type = (SettingsType)valObj["t"];
		}
	}
	return type;
}

bool Settings::IsKey(const char* key) {
	return (mInitialized && key != 0 && mDoc[key]);
}

/* Get a key value */
int8_t Settings::GetChar(const char* key, const int8_t defaultValue) {
	JsonVariant value = GetValue(key, SettingsType::PT_I8);
	return (value.is<signed char>()) ? value.as<signed char>() : defaultValue;
}

uint8_t Settings::GetUChar(const char* key, const uint8_t defaultValue) {
	JsonVariant value = GetInstanceData()->GetValue(key, SettingsType::PT_U8);
	return (value.is<unsigned char>()) ? value.as<unsigned char>() : defaultValue;
}

int16_t Settings::GetShort(const char* key, const int16_t defaultValue) {
	JsonVariant value = GetInstanceData()->GetValue(key, SettingsType::PT_I16);
	return (value.is<signed short>()) ? value.as<signed short>() : defaultValue;
}

uint16_t Settings::GetUShort(const char* key, const uint16_t defaultValue) {
	JsonVariant value = GetInstanceData()->GetValue(key, SettingsType::PT_U16);
	return (value.is<unsigned short>()) ? value.as<unsigned short>() : defaultValue;
}

int32_t Settings::GetInt(const char* key, const int32_t defaultValue) {
	JsonVariant value = GetInstanceData()->GetValue(key, SettingsType::PT_I32);
	return (value.is<signed int>()) ? value.as<signed int>() : defaultValue;
}

uint32_t Settings::GetUInt(const char* key, const uint32_t defaultValue) {
	JsonVariant value = GetInstanceData()->GetValue(key, SettingsType::PT_U32);
	return (value.is<unsigned int>()) ? value.as<unsigned int>() : defaultValue;
}

int32_t Settings::GetLong(const char* key, const int32_t defaultValue) {
	return GetInt(key, defaultValue);
}

uint32_t Settings::GetULong(const char* key, const uint32_t defaultValue) {
	return GetUInt(key, defaultValue);
}

int64_t Settings::GetLong64(const char* key, const int64_t defaultValue) {
	JsonVariant value = GetInstanceData()->GetValue(key, SettingsType::PT_I64);
	return (value.is<signed long long>()) ? value.as<signed long long>() : defaultValue;
}

uint64_t Settings::GetULong64(const char* key, const uint64_t defaultValue) {
	JsonVariant value = GetInstanceData()->GetValue(key, SettingsType::PT_U64);
	return (value.is<unsigned long long>()) ? value.as<unsigned long long>() : defaultValue;
}

float_t Settings::GetFloat(const char* key, const float_t defaultValue) {
	JsonVariant value = GetInstanceData()->GetValue(key, SettingsType::PT_FLOAT);
	return (value.is<float>()) ? value.as<float>() : defaultValue;
}

double_t Settings::GetDouble(const char* key, const double_t defaultValue) {
	JsonVariant value = GetInstanceData()->GetValue(key, SettingsType::PT_DOUBLE);
	return (value.is<double>()) ? value.as<double>() : defaultValue;
}

bool Settings::GetBool(const char* key, const bool defaultValue) {
	JsonVariant value = GetInstanceData()->GetValue(key, SettingsType::PT_BOOL);
	return (value.is<bool>()) ? value.as<bool>() : defaultValue;
}

size_t Settings::GetString(const char* key, char* value, const size_t maxLen) {
	return GetInstanceData()->GetBlob(key, value, maxLen, true);
}

String Settings::GetString(const char* key, const String defaultValue) {
	JsonVariant value = GetInstanceData()->GetValue(key, SettingsType::PT_STR);
	return (value.is<String>()) ? value.as<String>() : defaultValue;
}

size_t Settings::GetBytesLength(const char* key) {
	JsonVariant value = GetInstanceData()->GetValue(key, SettingsType::PT_STR);
	return (value.is<String>()) ? value.as<String>().length() : 0;
}

size_t Settings::GetBytes(const char* key, void* buf, size_t maxLen) {
	return GetInstanceData()->GetBlob(key, buf, maxLen, false);
}

size_t Settings::FreeEntries() {
	return 2048;
}

Settings::SettingsInstanceData* Settings::GetInstanceData() {
	return (SettingsInstanceData*)mInstanceData;
}

} // namespace System

// #endif
