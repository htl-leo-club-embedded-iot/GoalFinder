#pragma once

#include <WString.h>

class Logger {
  public:
    enum class LogLevel { OK, DEBUG, INFO, WARN, ERROR };

    static void begin(unsigned long baudRate = 115200) {}

    static void Log(const String &message) {}
    static void log(const String &message, LogLevel level) {}
    static void log(const String &message, const String &file, LogLevel level) {}
    static void logExtra(const String &message, const String &file, LogLevel level) {}

    static void log(const char *file, LogLevel level, const char *fmt, ...) {}
    static void LogExtra(const char *file, LogLevel level, const char *fmt, ...) {}

    static void Loop() {}
};
