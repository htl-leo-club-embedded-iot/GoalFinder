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
#include "GameManager.h"
#include "util/Logger.h"

// Hardware pins and constants
const int GoalFinderApp::pSda = 22;
const int GoalFinderApp::pScl = 21;
const int GoalFinderApp::pI2SBitClock = 23;
const int GoalFinderApp::pI2SWordClock = 5;
const int GoalFinderApp::pI2SDataOut = 19;
const int GoalFinderApp::pLedPwm = 17;
const int GoalFinderApp::pRadom = 36;

const int GoalFinderApp::ledPwmChannel = 0;

const int GoalFinderApp::shotVibrationThreshold = 2000;
const int GoalFinderApp::maxShotDurationMs = 5000;
const int GoalFinderApp::shotReadTimeout = 20;
const int GoalFinderApp::shotReadISRDuration = 20;
const int GoalFinderApp::hitReadTimeout = 20;

const char* GoalFinderApp::hitClips[] = { "/hit-1.mp3", "/hit-2.mp3", "/hit-3.mp3" };
const char* GoalFinderApp::tickClips[] = { "/tick-1.mp3", "/tick-2.mp3", "/tick-3.mp3", "/tick-4.mp3" };
const char* GoalFinderApp::missClips[] = { "/miss-1.mp3", "/miss-2.mp3", "/miss-3.mp3" };
const char* GoalFinderApp::waitingClips[] = { "/waiting-1.mp3", "/waiting-2.mp3", "/waiting-3.mp3" };

// FreeRTOS Handles
TaskHandle_t GoalFinderApp::TaskAudioHandle = nullptr;
TaskHandle_t GoalFinderApp::TaskDetectionHandle = nullptr;
TaskHandle_t GoalFinderApp::TaskLedHandle = nullptr;
TaskHandle_t GoalFinderApp::TaskWiFiHandle = nullptr;
TaskHandle_t GoalFinderApp::TaskLoggerHandle = nullptr;
TaskHandle_t GoalFinderApp::TaskDNSHandle = nullptr;
TaskHandle_t GoalFinderApp::TaskWebSocketHandle = nullptr;
TaskHandle_t GoalFinderApp::TaskHttpHandle = nullptr;
SemaphoreHandle_t GoalFinderApp::xMutex = nullptr;
volatile bool g_audioPlaybackActive = false;

GoalFinderApp::GoalFinderApp() :
    Singleton<GoalFinderApp>(),
    fileSystem(true),
    httpServer(&fileSystem),
    webSocket(),
    sntp(),
    audioPlayer(&fileSystem, pI2SBitClock, pI2SWordClock, pI2SDataOut),
    tofSensor(),
    sw420Sensor(),
    ledController(pLedPwm, ledPwmChannel),
    announcing(false),
    announcingUntilMs(0),
    lastMetronomeTickTime(0),
    announcement(Announcement::None),
    state(IDLE),
    lastShookTime(0),
    lastHitTime(0),
    afterHitTimeoutMs(5000),
    isSoundEnabled(true),
    isDetecting(false),
    distanceOnlyHitDetection(false),
    waitingSoundPlayCount(0)
{
}

GoalFinderApp::~GoalFinderApp() {}

void GoalFinderApp::SetIsSoundEnabled(bool value) {
    isSoundEnabled = value;
}

bool GoalFinderApp::IsSoundEnabled() {
    return isSoundEnabled;
}

void GoalFinderApp::SetIsDetecting(bool value) {
    isDetecting = value;
}

bool GoalFinderApp::IsDetecting() {
    return isDetecting;
}

// Initializing
void GoalFinderApp::Init() {
    delay(100);
    Serial.begin(115200);
    Logger::Begin(115200);

    randomSeed(analogRead(pRadom));

    if (fileSystem.Begin()) {
        wifiManager.Init();
        delay(200);

        httpServer.Begin();
        webSocket.Begin();
        sntp.Init();
        sw420Sensor.Init();
        tofSensor.Init(pScl, pSda);
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

void GoalFinderApp::UpdateSettings(bool force) {
    Settings* settings = Settings::GetInstance();
    if (force || settings->IsModified()) {
        audioPlayer.SetVolume(settings->GetVolume());
        ledController.SetMode(settings->GetLedMode());
        distanceOnlyHitDetection = settings->GetDistanceOnlyHitDetection();
        afterHitTimeoutMs = settings->GetAfterHitTimeout() * 1000UL;
        settings->ClearModifiedState();
    }
}

// Tasks
void GoalFinderApp::TaskAudio(void *pvParameters) {
    GoalFinderApp* app = (GoalFinderApp*)pvParameters;
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

void GoalFinderApp::TaskDetection(void *pvParameters) {
    GoalFinderApp* app = (GoalFinderApp*)pvParameters;
    unsigned long lastTimerTick = 0;
    while (app->loop) {
        if (app->IsDetecting()) {
            app->UpdateSettings();
            app->DetectHit();
            app->ProcessAnnouncement();

            unsigned long now = millis();
            if (now - lastTimerTick >= 1000) {
                lastTimerTick = now;
                GameManager::GetInstance()->TickTimer();
            }
        }
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

void GoalFinderApp::TaskLed(void *pvParameters) {
    GoalFinderApp* app = (GoalFinderApp*)pvParameters;
    while (app->loop) {
        app->ledController.Loop();
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

void GoalFinderApp::TaskWiFi(void *pvParameters) {
    GoalFinderApp* app = (GoalFinderApp*)pvParameters;
    while (app->loop) {
        app->wifiManager.Loop();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void GoalFinderApp::TaskLogger(void *pvParameters) {
    GoalFinderApp* app = (GoalFinderApp*)pvParameters;
    while (app->loop) {
        Logger::Loop();
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

void GoalFinderApp::TaskWebSocket(void *pvParameters) {
    GFWebSocket* ws = (GFWebSocket*)pvParameters;
    while (true) {
        ws->Loop();
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

void GoalFinderApp::TaskHttp(void *pvParameters) {
    HttpServer* http = (HttpServer*)pvParameters;
    while (true) {  
        http->Loop();
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

void GoalFinderApp::TickMetronome() {
    unsigned long currentTime = millis();
    if ((currentTime - lastMetronomeTickTime) > Settings::GetInstance()->GetMetronomeTiming()) {
        lastMetronomeTickTime = currentTime;
        const char* clipName = 0;

        bool useWaitingSound = (lastShookTime > 0);
        if (useWaitingSound) {
            if (waitingSoundPlayCount < 3) {
                clipName = waitingClips[Settings::GetInstance()->GetWaitingSound()];
                waitingSoundPlayCount++;
            }
        } else {
            clipName = tickClips[Settings::GetInstance()->GetMetronomeSound()];
        }
        PlaySound(clipName);
    }
}

void GoalFinderApp::DetectHit() {
    bool inCooldown = lastHitTime > 0 && (millis() - lastHitTime) < afterHitTimeoutMs;

    if (!inCooldown) {
        if (ReadHit()) {
            lastHitTime = millis();
            lastShookTime = 0;
            state = IDLE;
            AnnounceHit();
        } else if (!distanceOnlyHitDetection) {
            if (state == SHOT_DETECTED && millis() - lastShookTime > maxShotDurationMs) {
                lastHitTime = millis();
                lastShookTime = 0;
                state = IDLE;
                AnnounceMiss();
            } else if (state == IDLE && ReadShot()) {
                lastShookTime = millis();
                state = SHOT_DETECTED;
                OnShotDetected();
                for (int i = 0; i < 3; i++) {
                    if (ReadHit()) {
                        lastHitTime = millis();
                        lastShookTime = 0;
                        state = IDLE;
                        AnnounceHit();
                        break;
                    }
                }
            }
        }
    }
}

void GoalFinderApp::OnShotDetected() {
    announcement = Announcement::Shot;
    waitingSoundPlayCount = 0;
    ledController.OnShotDetected();
    Logger::Log("GoalfinderApp", Logger::LogLevel::INFO, "Shot detected");
}

bool GoalFinderApp::ReadShot() {
    bool result = false;
    static unsigned long lastReadTime = 0;

    if (millis() - lastReadTime > shotReadTimeout) {
        result = ReadShotISR() > Settings::GetInstance()->GetVibrationSensorSensitivity() / 25;
        lastReadTime = millis();
    }

    return result;
}

unsigned int GoalFinderApp::ReadShotISR() {
    unsigned long isrBegin = millis();
    unsigned int edges = 0;
    bool lastState = sw420Sensor.GetState();

    vTaskSuspendAll();
    while (millis() - isrBegin < shotReadISRDuration) {
        bool currentState = sw420Sensor.GetState();
        if(currentState != lastState) {
            lastState = currentState;
            edges++;
        }
    }
    
    xTaskResumeAll();
    return edges;
}

bool GoalFinderApp::ReadHit() {
    bool result = false;
    static unsigned long lastReadTime = 0;
    int distance = tofSensor.ReadSingleMillimeters();
    unsigned int distanceRequired = Settings::GetInstance()->GetBallHitDetectionDistance();
    unsigned int minDist = 100;
    lastRawDistance = distance;

    if (millis() - lastReadTime > hitReadTimeout && distance != -1 && distance > minDist && distance < distanceRequired) {
        result = true;        
        lastReadTime = millis();
    } /* else if (state == SHOT_DETECTED) {
        Logger::Log("GoalfinderApp", Logger::LogLevel::INFO, "SHOT_DETECTED ReadHit: dist=%d req=%d min=%d", distance, distanceRequired, minDist);
    } */

    return result;
}

void GoalFinderApp::ProcessAnnouncement() {
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

void GoalFinderApp::AnnounceHit() {
    announcement = Announcement::Hit;
    ledController.OnHit();

    GameManager* gm = GameManager::GetInstance();
    if (gm->GetSession()->isRunning) {
        gm->RecordHit(gm->GetSession()->currentPlayerIndex);
    }

    webSocket.SendHitEvent();
    webSocket.BroadcastGameState();
}

void GoalFinderApp::AnnounceMiss() {
    announcement = Announcement::Miss;
    ledController.OnMiss();

    GameManager* gm = GameManager::GetInstance();
    if (gm->GetSession()->isRunning) {
        gm->RecordMiss(gm->GetSession()->currentPlayerIndex);
    }

    webSocket.SendMissEvent();
    webSocket.BroadcastGameState();
}

void GoalFinderApp::AnnounceEvent(const char* traceMsg, const char* sound, unsigned long timeoutMs) {
    Logger::Log("GoalfinderApp", Logger::LogLevel::INFO, "Event: '%s'", traceMsg);
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

void GoalFinderApp::PlaySound(const char* soundFileName) {
    if (soundFileName) {
        Logger::LogExtra("GoalfinderApp", Logger::LogLevel::INFO, "Starting playback '%s'", soundFileName);

        if (xMutex == nullptr) {
            Logger::Log("GoalfinderApp", Logger::LogLevel::WARN,
                        "Cannot play '%s': audio mutex is null", soundFileName);
            return;
        }

        if (xSemaphoreTake(xMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
            audioPlayer.Stop();
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

void GoalFinderApp::Process() {
    delay(1);
}

void GoalFinderApp::SetDNSEnabled(bool enable) {
    dnsServer.IsRunning = enable;
}