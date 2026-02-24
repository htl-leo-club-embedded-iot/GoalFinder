
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

#ifndef ___LOGGER_H
#define ___LOGGER_H

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

private:
    static LogLevel currentLevel;
    static QueueHandle_t logQueue;

    static const char* levelToString(LogLevel level);
    static void printNow(const LogEntry &entry);

    static void printFormatted(const String &message,
                               const String &file,
                               LogLevel level);

    static void enqueue(const String &message,
                        const String &file,
                        LogLevel level);
};

#endif