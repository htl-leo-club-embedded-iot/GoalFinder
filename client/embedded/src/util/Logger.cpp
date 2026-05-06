
/* ===============================================================================
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
 * =============================================================================== */

#include "Logger.h"


Logger::LogLevel Logger::currentLevel = Logger::LogLevel::DEBUG;
QueueHandle_t Logger::logQueue = nullptr;
QueueHandle_t Logger::webLogQueue = nullptr;

void Logger::Begin(unsigned long baudRate) {
    Serial.begin(baudRate);
    while (!Serial) { }

    logQueue = xQueueCreate(50, sizeof(LogEntry*));
    if (logQueue == nullptr) {
        Serial.println("[ERROR][Logger] failed to create log queue");
    }

    webLogQueue = xQueueCreate(50, sizeof(String*));
    if (webLogQueue == nullptr) {
        Serial.println("[ERROR][Logger] failed to create web log queue");
    }
}

const char* Logger::LevelToString(Logger::LogLevel level) {
    switch (level) {
        case Logger::LogLevel::OK:     return "OK";
        case Logger::LogLevel::DEBUG:  return "DEBUG";
        case Logger::LogLevel::INFO:   return "INFO";
        case Logger::LogLevel::WARN:   return "WARN";
        case Logger::LogLevel::ERROR:  return "ERROR";
        default:                             return "UNKNOWN";
    }
}

void Logger::PrintFormatted(const String &message, const String &file, Logger::LogLevel level) {
    Enqueue(message, file, level);
}

void Logger::Log(const String &message) {
    PrintFormatted(message, "unknown", Logger::LogLevel::INFO);
}

void Logger::Log(const String &message, Logger::LogLevel level) {
    PrintFormatted(message, "unknown", level);
}

void Logger::Log(const String &message, const String &file, Logger::LogLevel level) {
    PrintFormatted(message, file, level);
}

void Logger::LogExtra(const String &message, const String &file, Logger::LogLevel level) {
    if (Settings::GetInstance()->GetExtraLog()) {
        PrintFormatted(message, file, level);
    }
}

void Logger::Log(const char *file, Logger::LogLevel level, const char *fmt, ...) {
    char buf[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    PrintFormatted(String(buf), String(file), level);
}

void Logger::LogExtra(const char *file, Logger::LogLevel level, const char *fmt, ...) {
    if (Settings::GetInstance()->GetExtraLog()) {
        char buf[256];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buf, sizeof(buf), fmt, args);
        va_end(args);
        PrintFormatted(String(buf), String(file), level);
    }
}

void Logger::Loop() {
    if (logQueue != nullptr) {
        LogEntry *entryPtr = nullptr;
        while (xQueueReceive(logQueue, &entryPtr, 0) == pdTRUE) {
            if (entryPtr != nullptr) {
                PrintNow(*entryPtr);
                delete entryPtr;
                entryPtr = nullptr;
            }
        }
    }
}

void Logger::DrainWebLogQueue() {
    if (webLogQueue != nullptr) {
        String* messagePtr = nullptr;
        while (xQueueReceive(webLogQueue, &messagePtr, 0) == pdTRUE) {
            if (messagePtr != nullptr) {
                GFWebSocket& ws = GoalfinderApp::GetInstance()->GetWebSocket();
                ws.SendWebLog(*messagePtr);
                delete messagePtr;
                messagePtr = nullptr;
            }
        }
    }
}

void Logger::PrintNow(const LogEntry &entry) {
    String out = String("[") + LevelToString(entry.level) + "]";
    if (!entry.file.isEmpty()) {
        out += String("[") + entry.file + "]";
    }
    out += " " + entry.message;

    if (webLogQueue != nullptr) {
        String* outPtr = new String(out);
        if (xQueueSend(webLogQueue, &outPtr, 0) != pdTRUE) {
            delete outPtr;
        }
    }

    Serial.println(out);
}

void Logger::Enqueue(const String &message, const String &file, Logger::LogLevel level) {
    if (logQueue == nullptr) {
        PrintNow({message, file, level});
    } else {   
        LogEntry *entryPtr = new LogEntry{message, file, level};
        if (xQueueSend(logQueue, &entryPtr, 0) != pdTRUE) {
            PrintNow(*entryPtr);
            delete entryPtr;
        }
    }
}