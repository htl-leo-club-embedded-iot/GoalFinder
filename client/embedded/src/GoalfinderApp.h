/*
 * ===============================================================================
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
 * ===============================================================================
 */

#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <Singleton.h>
#include <VL53L0X.h>
#include <web/SNTP.h>
#include <FileSystem.h>
#include <AudioPlayer.h>
#include <LedController.h>
#include <util/Logger.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include <SW420.h>
#include <web/HttpServer.h>
#include <web/WebSocket.h>
#include "web/WiFiManager.h"
#include "web/DNSServer.h"

/**
 * GoalFinderApp - Main application class for the GoalFinder goal detection system.
 *
 * Orchestrates all subsystems (ToF ranging, vibration sensing, audio playback,
 * LED feedback, WiFi, HTTP/WebSocket serving) across multiple FreeRTOS tasks.
 * Runs on an ESP32 and detects soccer/football goals, announcing results via
 * sound clips and WebSocket push events.
 */
class GoalFinderApp : public Singleton<GoalFinderApp> {
public:
    // Public interface

    /**
     * Globally enable or disable all sound output.
     * When disabled, metronome ticks, hit/miss sounds, and waiting sounds
     * are all suppressed. LED feedback continues regardless.
     */
    void SetIsSoundEnabled(bool value);

    /** @return true if sound output is currently enabled. */
    bool IsSoundEnabled();

    /** Destructor */
    virtual ~GoalFinderApp();

    /**
     * Initializes all hardware and software subsystems and spawns FreeRTOS tasks.
     *
     * Must be called once before the scheduler runs. Sets up I2C (ToF sensor),
     * I2S (audio), GPIO (LED PWM, vibration sensor), file system, WiFi in AP+STA
     * mode, HTTP/WebSocket servers, SNTP, and captive-portal DNS. Then creates
     * eight FreeRTOS tasks pinned to appropriate cores.
     */
    void Init();

    /**
     * Single iteration step for the main loop.
     *
     * Currently unused; all work is delegated to FreeRTOS tasks after Init().
     * Retained for compatibility with potential polling-based usage.
     */
    void Process();

    /**
     * Attempts to play the next metronome tick if the configured interval has elapsed.
     *
     * Switches to a "waiting" sound clip when a shot was previously detected
     * (lastShookTime > 0) but no hit followed, providing audible feedback that
     * the system is listening for a hit.
     */
    void TickMetronome();

    /**
     * Runs one detection cycle for shot/hit/miss.
     *
     * Called every ~5 ms from TaskDetection. Reads the ToF distance sensor and,
     * depending on configuration, also the vibration sensor. Enforces a cooldown
     * window after hits (afterHitTimeoutMs) to prevent rapid re-triggering.
     */
    void DetectHit();

    /**
     * Dispatches the current pending announcement and resets the announcement flag.
     *
     * Called every ~5 ms from TaskDetection after DetectHit(). Reads the
     * Announcement enum and plays the appropriate sound clip while sending
     * the corresponding WebSocket event. Guarantees each detection fires exactly
     * one announcement cycle.
     */
    void ProcessAnnouncement();

    // Public hardware access

    /** Audio player: MP3 decode and I2S output. */
    AudioPlayer audioPlayer;

    /** LED controller: visual feedback animations (flash, pulse, breathe). */
    LedController ledController;

    // FreeRTOS task entry points (static, passed as pvParameters)

    /** Audio task: pumps audio decode loop, ticks metronome. Core 1, prio 3. */
    static void TaskAudio(void *pvParameters);

    /** Detection task: polls sensors, processes events. Core 1, prio 2. */
    static void TaskDetection(void *pvParameters);

    /** LED task: advances LED animation state. Core 1, prio 2. */
    static void TaskLed(void *pvParameters);

    /** WiFi task: maintains AP/STA connectivity and captive portal. Core 0, prio 1. */
    static void TaskWiFi(void *pvParameters);

    /** Logger task: flushes buffered log output. Core 1, prio 1. */
    static void TaskLogger(void *pvParameters);

    /** WebSocket task: processes client messages. Core 0, prio 3. */
    static void TaskWebSocket(void *pvParameters);

    /** HTTP task: serves the configuration web interface. Core 0, prio 3. */
    static void TaskHttp(void *pvParameters);

    /**
     * Enable or disable the captive-portal DNS server.
     * When enabled, all DNS requests are answered with the device's IP so
     * clients are redirected to the configuration interface automatically.
     */
    void SetDNSEnabled(bool enabled);

    /**
     * Records that at least one WebSocket client has connected during this
     * power cycle. Used internally to suppress redundant log messages about
     * client connect/disconnect.
     */
    void NotifyWebSocketClientConnected();

    /** @return Reference to the WebSocket server instance. */
    GFWebSocket& GetWebSocket() { return webSocket; }

private:
    friend class Singleton<GoalFinderApp>;
    /** Singleton – use GoalFinderApp::GetInstance() instead. */
    GoalFinderApp();

    // Pin assignments

    static const int pSda;      /**< I2C data for VL53L0X ToF sensor. */
    static const int pScl;      /**< I2C clock for VL53L0X ToF sensor. */
    static const int pI2SBitClock;     /**< I2S bit clock. */
    static const int pI2SWordClock;     /**< I2S word (left/right) clock. */
    static const int pI2SDataOut;  /**< I2S serial data output. */
    static const int pLedPwm;      /**< LED indicator PWM output. */
    static const int pRadom;  /**< Analog input for seeding random(). */

    static const int ledPwmChannel;  /**< LEDC channel used for the LED PWM pin. */

    /** Audio clip arrays – one entry is picked at random per event type. */
    static const char* hitClips[];
    static const char* tickClips[];
    static const char* missClips[];
    static const char* waitingClips[];

    /**
     * FSM states for the shot→hit→miss detection flow.
     *
     * IDLE -> SHOT_DETECTED (vibration threshold crossed)
     * SHOT_DETECTED -> HIT_DETECTED (ToF reads ball within goal)
     * SHOT_DETECTED -> IDLE (timeout without hit = miss)
     */
    enum State {
        IDLE,
        SHOT_DETECTED,
        HIT_DETECTED
    };

    /** Current detection FSM state. */
    State state;

    // Internal event handlers

    /** Called when the vibration sensor crosses the shot threshold. */
    void OnShotDetected();

    /**
     * Triggers a "hit" announcement: random hit sound clip, WebSocket push,
     * and post-hit cooldown to prevent double-triggering.
     */
    void AnnounceHit();

    /**
     * Triggers a "miss" announcement: random miss sound clip and WebSocket
     * push.
     */
    void AnnounceMiss();

    /**
     * Dispatches an announcement event with optional audio playback.
     *
     * @param traceMsg  Label used in log output for this event type.
     * @param sound     Path to the MP3 file to play, or nullptr for silent.
     * @param timeoutMs How long to suppress new detections while announcing
     *                  (clamped by audio finishing early via !IsPlaying()).
     */
    void AnnounceEvent(const char* traceMsg, const char* sound, unsigned long timeoutMs = 3000UL);

    /**
     * Starts MP3 playback through the audio player.
     *
     * Acquires the audio mutex with a 50 ms timeout. If the mutex cannot
     * be taken (audio task is actively decoding), the playback request is
     * dropped to avoid blocking the calling task.
     */
    void PlaySound(const char* soundFileName);

    /**
     * Reloads settings from the Settings singleton if they have been modified
     * (or unconditionally when force=true). Called every detection cycle so
     * web UI changes take effect immediately.
     */
    void UpdateSettings(bool force = false);

    /** Configures WiFi in AP+STA mode using saved credentials. */
    void WiFiSetup();

    /**
     * Scans for nearby WiFi networks and applies a device name based on the
     * strongest signal. Ensures each unit gets a unique default hostname.
     */
    void ApplyDeviceNameByScan();

    /**
     * Reads the vibration sensor with a polling-based debounce.
     *
     * @return true if the edge count exceeds the sensitivity-derived threshold.
     */
    bool ReadShot();

    /**
     * Busy-waits on the vibration sensor GPIO for shotReadISRDuration ms,
     * counting rising/falling edges. Suspends the scheduler during the loop
     * for precise timing.
     *
     * @return Number of edges detected during the sampling window.
     */
    unsigned int ReadShotISR();

    /**
     * Reads the distance sensor with a polling-based debounce.
     *
     * @return true if the required count of distance reading in a range of ±30 is reached.
     */
    bool ReadHit();

    // Internal subsystem objects

    FileSystem fileSystem;       /**< SPIFFS / LittleFS file system. */
    HttpServer httpServer;       /**< REST API for web configuration. */
    GFWebSocket webSocket;       /**< Real-time event push to web clients. */
    SNTP sntp;                   /**< NTP time synchronization. */
    WiFiManager wifiManager;     /**< WiFi credential management. */
    GFDNSServer dnsServer;       /**< Captive-portal DNS responder. */
    VL53L0X tofSensor;           /**< Time-of-Flight distance sensor. */
    SW420 sw420Sensor;           /**< Vibration / shock sensor. */

    // Internal state

    bool isSoundEnabled;               /**< Master sound enable flag. */
    bool announcing;                   /**< True while an announcement sound is playing or its timeout is active. */
    unsigned long announcingUntilMs;   /**< millis() deadline after which the announcement cooldown expires. */
    bool distanceOnlyHitDetection;     /**< If true, skip vibration sensor; use only ToF for hit detection. */
    unsigned long lastMetronomeTickTime; /**< millis() of the last metronome tick, used to enforce timing. */
    unsigned long lastShookTime;       /**< millis() when shot vibration was last detected; 0 means no pending shot. */
    unsigned long lastHitTime;         /**< millis() of the last confirmed hit, used for post-hit cooldown. */
    unsigned long afterHitTimeoutMs;   /**< Duration in ms to suppress detection after a hit. */
    uint8_t waitingSoundPlayCount; /**< The maximum number of waiting sound clips to play when waiting for a hit */

    /**
     * Announcement event type – set by DetectHit() and consumed by ProcessAnnouncement().
     *
     * - None: no pending announcement
     * - Shot: shot detected, no sound played (event only)
     * - Hit:  goal scored, play hit sound + WebSocket event
     * - Miss: timeout without hit, play miss sound + WebSocket event
     */
    struct Announcement {
        typedef enum {
            None,
            Shot,
            Hit,
            Miss
        } Enum;
    };
    Announcement::Enum announcement;

    static const int shotVibrationThreshold;    /**< Minimum vibration edges to register a shot. */
    static const int maxShotDurationMs;         /**< Max time after a shot to wait for a hit before declaring miss. */
    static const int shotReadTimeout;           /**< Minimum interval between ReadShot() calls. */
    static const int shotReadISRDuration;       /**< How long ReadShotISR() samples the vibration sensor (ms). */
    static const int hitReadTimeout;            /**< Minimum interval between ReadHit */

    // FreeRTOS handles (static)

    static TaskHandle_t TaskAudioHandle;
    static TaskHandle_t TaskDetectionHandle;
    static TaskHandle_t TaskLedHandle;
    static TaskHandle_t TaskWiFiHandle;
    static TaskHandle_t TaskLoggerHandle;
    static TaskHandle_t TaskDNSHandle;
    static TaskHandle_t TaskWebSocketHandle;
    static TaskHandle_t TaskHttpHandle;
    static SemaphoreHandle_t xMutex;  /**< Mutex protecting audio player access across tasks. */

    /** Loop control flag – set to false to stop all tasks gracefully. */
    bool loop = true;
};