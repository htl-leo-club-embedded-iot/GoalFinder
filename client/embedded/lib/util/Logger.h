#pragma once

/* Lightweight declaration of Logger API for library builds.
 * This header mirrors the public interface of the real
 * `src/util/Logger.h` so files under `lib/` can include
 * "util/Logger.h" without requiring access to the `src/`
 * include path during library compilation.
 *
 * Keep this in sync with src/util/Logger.h.
 */

#include <WString.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

class Logger
{
public:
	enum class LogLevel
	{
		OK,
		DEBUG,
		INFO,
		WARN,
		ERROR
	};

	struct LogEntry {
		String message;
		String file;
		LogLevel level;
	};

	static void begin(unsigned long baudRate = 115200);

	static void log(const String &message);
	static void log(const String &message, LogLevel level);
	static void log(const String &message, const String &file, LogLevel level);
	static void logExtra(const String &message, const String &file, LogLevel level);

	static void log(const char *file, LogLevel level, const char *fmt, ...);
	static void logExtra(const char *file, LogLevel level, const char *fmt, ...);

	static void Loop();
};
