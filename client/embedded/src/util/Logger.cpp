
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
#include <Arduino.h>
#include "../Settings.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

Logger::LogLevel Logger::currentLevel = Logger::LogLevel::DEBUG;
QueueHandle_t Logger::logQueue = nullptr;

void Logger::begin(unsigned long baudRate)
{
    Serial.begin(baudRate);
    while (!Serial) { }

    logQueue = xQueueCreate(50, sizeof(LogEntry*));
    if (logQueue == nullptr) {
        Serial.println("[ERROR][Logger] failed to create log queue");
    }
}

const char* Logger::levelToString(Logger::LogLevel level)
{
    switch (level)
    {
        case Logger::LogLevel::OK:     return "OK";
        case Logger::LogLevel::DEBUG:  return "DEBUG";
        case Logger::LogLevel::INFO:   return "INFO";
        case Logger::LogLevel::WARN:   return "WARN";
        case Logger::LogLevel::ERROR:  return "ERROR";
        default:                             return "UNKNOWN";
    }
}

void Logger::printFormatted(const String &message, const String &file, Logger::LogLevel level)
{
    enqueue(message, file, level);
}

void Logger::log(const String &message)
{
    printFormatted(message, "unknown", Logger::LogLevel::INFO);
}

void Logger::log(const String &message, Logger::LogLevel level)
{
    printFormatted(message, "unknown", level);
}

void Logger::log(const String &message, const String &file, Logger::LogLevel level)
{
    printFormatted(message, file, level);
}

void Logger::logExtra(const String &message, const String &file, Logger::LogLevel level)
{
    if (Settings::GetInstance()->GetExtraLog()) {
        printFormatted(message, file, level);
    }
}

void Logger::log(const char *file, Logger::LogLevel level, const char *fmt, ...)
{
    char buf[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    printFormatted(String(buf), String(file), level);
}

void Logger::logExtra(const char *file, Logger::LogLevel level, const char *fmt, ...)
{
    if (Settings::GetInstance()->GetExtraLog()) {
        char buf[256];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buf, sizeof(buf), fmt, args);
        va_end(args);
        printFormatted(String(buf), String(file), level);
    }
}

void Logger::Loop()
{
    if (logQueue == nullptr) {
        return;
    }

    LogEntry *entryPtr = nullptr;
    if (xQueueReceive(logQueue, &entryPtr, 0) == pdTRUE && entryPtr != nullptr) {
        printNow(*entryPtr);
        delete entryPtr;
    }
}

void Logger::printNow(const LogEntry &entry)
{
    String out = String("[") + levelToString(entry.level) + "]";
    if (!entry.file.isEmpty()) {
        out += String("[") + entry.file + "]";
    }
    out += " " + entry.message;
    Serial.println(out);
}

void Logger::enqueue(const String &message, const String &file, Logger::LogLevel level)
{
    if (logQueue == nullptr) {
        printNow({message, file, level});
        return;
    }

    LogEntry *entryPtr = new LogEntry{message, file, level};
    if (xQueueSend(logQueue, &entryPtr, 0) != pdTRUE) {
        // queue full or failed — fallback to immediate print and free
        printNow(*entryPtr);
        delete entryPtr;
    }
}