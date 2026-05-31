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

#include <GoalfinderApp.h>
#include <HardwareSerial.h>
#include <Settings.h>
#include "util/Logger.h"

// Hardware pins and constants
const int GoalfinderApp::pinTofSda = 22;
const int GoalfinderApp::pinTofScl = 21;
const int GoalfinderApp::pinI2sBclk = 23;
const int GoalfinderApp::pinI2sWclk = 5;
const int GoalfinderApp::pinI2sDataOut = 19;
const int GoalfinderApp::pinLedPwm = 17;
const int GoalfinderApp::pinRandomSeed = 36;

const int GoalfinderApp::ledPwmChannel = 0;

const int GoalfinderApp::shotVibrationThreshold = 2000;
const int GoalfinderApp::maxShotDurationMs = 5000;

const char* GoalfinderApp::hitClips[] = { "/hit-1.mp3", "/hit-2.mp3", "/hit-3.mp3" };
const char* GoalfinderApp::tickClips[] = { "/tick-1.mp3", "/tick-2.mp3", "/tick-3.mp3", "/tick-4.mp3" };
const char* GoalfinderApp::missClips[] = { "/miss-1.mp3", "/miss-2.mp3", "/miss-3.mp3" };
const char* GoalfinderApp::waitingClips[] = { "/waiting-1.mp3", "/waiting-2.mp3", "/waiting-3.mp3" };

// FreeRTOS Handles
TaskHandle_t GoalfinderApp::TaskAudioHandle = nullptr;
TaskHandle_t GoalfinderApp::TaskDetectionHandle = nullptr;
TaskHandle_t GoalfinderApp::TaskLedHandle = nullptr;
TaskHandle_t GoalfinderApp::TaskWiFiHandle = nullptr;
TaskHandle_t GoalfinderApp::TaskLoggerHandle = nullptr;
TaskHandle_t GoalfinderApp::TaskDNSHandle = nullptr;
TaskHandle_t GoalfinderApp::TaskWebSocketHandle = nullptr;
TaskHandle_t GoalfinderApp::TaskHttpHandle = nullptr;
SemaphoreHandle_t GoalfinderApp::xMutex = nullptr;
volatile bool g_audioPlaybackActive = false;

// Constructor
GoalfinderApp::GoalfinderApp() :
    Singleton<GoalfinderApp>(),
    fileSystem(true),
    httpServer(&fileSystem),
    webSocket(),
    sntp(),
    audioPlayer(&fileSystem, pinI2sBclk, pinI2sWclk, pinI2sDataOut),
    tofSensor(),
    sw420Sensor(),
    ledController(pinLedPwm, ledPwmChannel),
    announcing(false),
    announcingUntilMs(0),
    lastMetronomeTickTime(0),
    announcement(Announcement::None),
    lastShookTime(0),
    lastHitTime(0),
    afterHitTimeoutMs(5000),
    isSoundEnabled(true),
    distanceOnlyHitDetection(false)
{}

GoalfinderApp::~GoalfinderApp() {}

void GoalfinderApp::SetIsSoundEnabled(bool value) {
    isSoundEnabled = value;
}

bool GoalfinderApp::IsSoundEnabled() {
    return isSoundEnabled;
}

// Initializing
void GoalfinderApp::Init() {
    delay(100);
    Serial.begin(115200);
    Logger::Begin(115200);

    randomSeed(analogRead(pinRandomSeed));

    if (fileSystem.Begin()) {
        wifiManager.Init();
        delay(200);

        httpServer.Begin();
        webSocket.Begin();
        sntp.Init();
        sw420Sensor.Init(Settings::GetInstance()->GetVibrationSensorSensitivity());
        tofSensor.Init(pinTofScl, pinTofSda);
        ledController.SetMode(LedMode::Flash);

        UpdateSettings(true);

        xMutex = xSemaphoreCreateMutex();    

        Settings* settings = Settings::GetInstance();
        String configuredDeviceIp = settings->GetDeviceIpAddress();
        IPAddress deviceIP;
        if (!deviceIP.fromString(configuredDeviceIp)) {
            Logger::Log("GoalfinderApp", Logger::LogLevel::WARN,
                "Configured device IP '%s' is invalid. Falling back to 192.168.4.1",
                configuredDeviceIp.c_str());
            configuredDeviceIp = "192.168.4.1";
            deviceIP.fromString(configuredDeviceIp);
            settings->SetDeviceIpAddress(configuredDeviceIp);
        }

        if ((WiFi.getMode() & WIFI_AP) != 0) {
            IPAddress apIp = WiFi.softAPIP();
            if (apIp != IPAddress(static_cast<uint32_t>(0))) {
                deviceIP = apIp;
            }
        }

        dnsServer.Begin(deviceIP, settings->GetDeviceName());
        dnsServer.IsRunning = settings->DNSEnabled();

        xTaskCreatePinnedToCore(TaskAudio,          "Audio",     8192, this,           3, &TaskAudioHandle,     1);
        xTaskCreatePinnedToCore(TaskDetection,      "Detection", 8192, this,           2, &TaskDetectionHandle, 1);
        xTaskCreatePinnedToCore(TaskLed,            "LED",       8192, this,           2, &TaskLedHandle,       1);
        xTaskCreatePinnedToCore(TaskLogger,         "Logger",    4096, this,           1, &TaskLoggerHandle,    1);
        xTaskCreatePinnedToCore(TaskWiFi,           "WiFi",      4096, this,           1, &TaskWiFiHandle,      0);
        xTaskCreatePinnedToCore(GFDNSServer::Task,  "DNS",       4096, &dnsServer,     1, &TaskDNSHandle,       0);
        xTaskCreatePinnedToCore(TaskWebSocket,      "WS",        8192, &webSocket,     3, &TaskWebSocketHandle, 0);
        xTaskCreatePinnedToCore(TaskHttp,           "HTTP",      8192, &httpServer,    3, &TaskHttpHandle,      0);

        Logger::Log("GoalfinderApp", Logger::LogLevel::OK, "All tasks started");
    } else {
        Logger::Log("GoalfinderApp", Logger::LogLevel::ERROR, "FS initialization failed");
    }
}

void GoalfinderApp::UpdateSettings(bool force) {
    Settings* settings = Settings::GetInstance();
    if (force || settings->IsModified()) {
        audioPlayer.SetVolume(settings->GetVolume());
        ledController.SetMode(settings->GetLedMode());
        sw420Sensor.SetSensitivity(settings->GetVibrationSensorSensitivity());
        distanceOnlyHitDetection = settings->GetDistanceOnlyHitDetection();
        afterHitTimeoutMs = settings->GetAfterHitTimeout() * 1000UL;
    }
}

// Tasks
void GoalfinderApp::TaskAudio(void *pvParameters) {
    GoalfinderApp* app = (GoalfinderApp*)pvParameters;
    while (app->loop) {
        bool isPlaying = g_audioPlaybackActive;

        if (app->IsSoundEnabled()) {
            if (!g_httpServingFile && xSemaphoreTake(xMutex, pdMS_TO_TICKS(10)) == pdTRUE) {
                app->audioPlayer.Loop();
                isPlaying = app->audioPlayer.IsPlaying();
                xSemaphoreGive(xMutex);
                if (!isPlaying) {
                    app->TickMetronome();
                }
            }
        } else {
            isPlaying = false;
        }

        g_audioPlaybackActive = isPlaying;

        if (g_httpServingFile || isPlaying) {
            vTaskDelay(pdMS_TO_TICKS(1));
        } else {
            vTaskDelay(pdMS_TO_TICKS(5));
        }
    }
}

void GoalfinderApp::TaskDetection(void *pvParameters) {
    GoalfinderApp* app = (GoalfinderApp*)pvParameters;
    while (app->loop) {
        app->UpdateSettings();
        app->DetectHit();
        app->ProcessAnnouncement();
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

void GoalfinderApp::TaskLed(void *pvParameters) {
    GoalfinderApp* app = (GoalfinderApp*)pvParameters;
    while (app->loop) {
        app->ledController.Loop();
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

void GoalfinderApp::TaskWiFi(void *pvParameters) {
    GoalfinderApp* app = (GoalfinderApp*)pvParameters;
    while (app->loop) {
        app->wifiManager.Loop();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void GoalfinderApp::TaskLogger(void *pvParameters) {
    GoalfinderApp* app = (GoalfinderApp*)pvParameters;
    while (app->loop) {
        Logger::Loop();
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

void GoalfinderApp::TaskWebSocket(void *pvParameters) {
    GFWebSocket* ws = (GFWebSocket*)pvParameters;
    while (true) {
        ws->Loop();
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

void GoalfinderApp::TaskHttp(void *pvParameters) {
    HttpServer* http = (HttpServer*)pvParameters;
    while (true) {  
        http->Loop();
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

// Play metronome sound
void GoalfinderApp::TickMetronome() {
    unsigned long currentTime = millis();
    if ((currentTime - lastMetronomeTickTime) > Settings::GetInstance()->GetMetronomeTiming()) {
        lastMetronomeTickTime = currentTime;

        int waitingSoundIndex = Settings::GetInstance()->GetWaitingSound();
        int metronomeSoundIndex = Settings::GetInstance()->GetMetronomeSound();
        int waitingClipCount = sizeof(waitingClips) / sizeof(waitingClips[0]);
        int tickClipCount = sizeof(tickClips) / sizeof(tickClips[0]);

        waitingSoundIndex = max(min(waitingSoundIndex, waitingClipCount - 1), 0);
        metronomeSoundIndex = max(min(metronomeSoundIndex, tickClipCount - 1), 0);

        bool useWaitingSound = (lastShookTime > 0);
        const char* clipName = useWaitingSound ? waitingClips[waitingSoundIndex] : tickClips[metronomeSoundIndex];
        PlaySound(clipName);
    }
}

void GoalfinderApp::DetectHit() {
    if (announcing && (millis() > announcingUntilMs || !audioPlayer.IsPlaying())) {
        announcing = false;
    }

    if (!(lastHitTime > 0 && (millis() - lastHitTime) < afterHitTimeoutMs)) {
        int distance = tofSensor.ReadSingleMillimeters();

        if (distanceOnlyHitDetection) {
            if (!(announcing && audioPlayer.IsPlaying())) {
                announcing = false;
                if (distance > 20 && distance < Settings::GetInstance()->GetBallHitDetectionDistance()) {
                    AnnounceHit();
                    lastHitTime = millis();
                }
            }
        } else {
            if (lastShookTime <= 0) {
                if (!(announcing && audioPlayer.IsPlaying())) {
                    announcing = false;
                    long vibration = sw420Sensor.Vibration(10000);
                    if (vibration > shotVibrationThreshold) {
                        lastShookTime = millis();
                        Logger::Log("GoalfinderApp", Logger::LogLevel::INFO, "Shot detected");
                    }
                }
            } else {
                unsigned long diff = millis() - lastShookTime;

                if (diff < maxShotDurationMs) {
                    int distance = tofSensor.ReadSingleMillimeters();
                    if (distance > 20 && distance < Settings::GetInstance()->GetBallHitDetectionDistance()) {
                        AnnounceHit();
                    }
                } else {
                    AnnounceMiss();
                }
                
                lastHitTime = millis();
                lastShookTime = 0;
            }
        }
    }
}

bool GoalfinderApp::ReadHit() {
    bool result = false;


    return result;
}

void GoalfinderApp::ProcessAnnouncement() {
    switch (announcement) {
        case Announcement::Shot:
            // no sound for Shot; keep announcing off
            AnnounceEvent("shot", nullptr, 0);
            break;
        case Announcement::Hit:
            // play hit sound and set a short timeout
            {
                int hitSoundIndex = Settings::GetInstance()->GetHitSound();
                int hitClipCount = sizeof(hitClips) / sizeof(hitClips[0]);
                hitSoundIndex = max(min(hitSoundIndex, hitClipCount - 1), 0);
                AnnounceEvent("hit", hitClips[hitSoundIndex], 2500UL);
            }
            break;
        case Announcement::Miss:
            {
                int missSoundIndex = Settings::GetInstance()->GetMissSound();
                int missClipCount = sizeof(missClips) / sizeof(missClips[0]);
                missSoundIndex = max(min(missSoundIndex, missClipCount - 1), 0);
                AnnounceEvent("miss", missClips[missSoundIndex], 3500UL);
            }
            break;
        default:
            break;
    }
    announcement = Announcement::None;
}

void GoalfinderApp::AnnounceHit() {
    announcement = Announcement::Hit;
    webSocket.SendHitEvent();
    Logger::Log("GoalfinderApp", Logger::LogLevel::OK, "Hit detected");
}

void GoalfinderApp::AnnounceMiss() {
    announcement = Announcement::Miss;
    webSocket.SendMissEvent();
    Logger::Log("GoalfinderApp", Logger::LogLevel::WARN, "Miss detected");
}

void GoalfinderApp::AnnounceEvent(const char* traceMsg, const char* sound, unsigned long timeoutMs) {
    Logger::Log("GoalfinderApp", Logger::LogLevel::INFO, "Announcing event '%s'", traceMsg);
    if (sound) {
        announcing = true;
        if (timeoutMs > 0) {
            announcingUntilMs = millis() + timeoutMs;
        } else {
            announcingUntilMs = 0;
        }
        PlaySound(sound);
    }
}

void GoalfinderApp::PlaySound(const char* soundFileName) {
    if (soundFileName) {
        Logger::LogExtra("GoalfinderApp", Logger::LogLevel::INFO, "Starting playback '%s'", soundFileName);

        if (xMutex == nullptr) {
            Logger::Log("GoalfinderApp", Logger::LogLevel::WARN,
                        "Cannot play '%s': audio mutex is null", soundFileName);
            return;
        }

        if (xSemaphoreTake(xMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
            audioPlayer.PlayMP3(soundFileName);

            if (!audioPlayer.IsPlaying()) {
                Logger::Log("GoalfinderApp", Logger::LogLevel::WARN,
                            "Playback did not start for '%s'", soundFileName);
            }

            xSemaphoreGive(xMutex);
        } else {
            Logger::Log("GoalfinderApp", Logger::LogLevel::WARN,
                        "Audio mutex timeout while starting '%s'", soundFileName);
        }
    }
}

void GoalfinderApp::Process() {
    delay(1);
}

void GoalfinderApp::SetDNSEnabled(bool enable) {
    dnsServer.IsRunning = enable;
}