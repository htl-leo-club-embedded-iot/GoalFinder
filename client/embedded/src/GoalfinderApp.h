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
#include <DNSServer.h>
#include <Singleton.h>
#include <ToFSensor.h>
#include <VibrationSensor.h>
#include <BluetoothManager.h>
#include <web/WebServer.h>
#include <web/SNTP.h>
#include <FileSystem.h>
#include <AudioPlayer.h>
#include <LedController.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

class GoalfinderApp : public Singleton<GoalfinderApp> {
public:
    // Public functions
    void SetIsSoundEnabled(bool value);
    bool IsSoundEnabled();

    int GetDetectedHits();
	int GetDetectedMisses();

	void ResetDetectedHits();
	void ResetDetectedMisses();

    /** Destructor */
    virtual ~GoalfinderApp();

    /** Initializes the application. */
    void Init();

    /** Processes one single iteration step (optional, mostly unused with tasks). */
    void Process();

    void TickMetronome();
    void DetectShot();
    void ProcessAnnouncement();

    // Public members
    AudioPlayer audioPlayer;
    LedController ledController;

    // Pins and constants
    static const int pinTofSda;
    static const int pinTofScl;
    static const int pinI2sBclk;
    static const int pinI2sWclk;
    static const int pinI2sDataOut;
    static const int pinLedPwm;
    static const int pinRandomSeed;

    static const int ledPwmChannel;

    static const int ballHitDetectionDistance;
    static const int shotVibrationThreshold;
    static const int maxShotDurationMs;

    static const char* waitingClip;
    static const char* hitClips[];
    static const int   hitClipsCnt;
    static const char* tickClips[];
    static const int   tickClipsCnt;
    static const char* missClips[];
    static const int   missClipsCnt;

    // FreeRTOS Tasks
    static void TaskAudioCode(void *pvParameters);
    static void TaskDetectionCode(void *pvParameters);
    static void TaskLedCode(void *pvParameters);

private:
    friend class Singleton<GoalfinderApp>;
    /** Singleton constructor */
    GoalfinderApp();

    // Private methods 
    void OnShotDetected();
    void AnnounceHit();
    void AnnounceMiss();
    void AnnounceEvent(const char* traceMsg, const char* sound);
    void PlaySound(const char* soundFileName);
    void UpdateSettings(bool force = false);
    void WiFiSetup();
    void ApplyDeviceNameByScan();

    // Internal objects
    FileSystem fileSystem;
    WebServer webServer;
    SNTP sntp;
    DNSServer dnsServer;
    ToFSensor tofSensor;
    VibrationSensor vibrationSensor;

    // Internal Values
    bool isSoundEnabled;
    bool announcing;
    bool distanceOnlyHitDetection;
    unsigned long lastMetronomeTickTime;
    unsigned long metronomeIntervalMs;
    unsigned long lastShockTime;

    // Events
    struct Announcement {
        typedef enum {
            None,   // No Event
            Shot,   
            Hit,    
            Miss    
        } Enum;
    };
    Announcement::Enum announcement;

    // Statistics
    int detectedHits = 0;
    int detectedMisses = 0;
    

    // FreeRTOS Handles
    static TaskHandle_t TaskAudio;
    static TaskHandle_t TaskDetection;
    static TaskHandle_t TaskLed;
    static SemaphoreHandle_t xMutex;
};