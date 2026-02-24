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

const char* GoalfinderApp::waitingClip = "/waiting.mp3";

const char* GoalfinderApp::hitClips[] = { "/hit-1.mp3", "/hit-2.mp3", "/hit-3.mp3" };
const int   GoalfinderApp::hitClipsCnt = sizeof(GoalfinderApp::hitClips) / sizeof(GoalfinderApp::hitClips[0]);

const char* GoalfinderApp::tickClips[] = { "/tick-1.mp3", "/tick-2.mp3", "/tick-3.mp3" };
const int   GoalfinderApp::tickClipsCnt = sizeof(GoalfinderApp::tickClips) / sizeof(GoalfinderApp::tickClips[0]);

const char* GoalfinderApp::missClips[] = { "/miss-1.mp3", "/miss-2.mp3", "/miss-3.mp3" };
const int   GoalfinderApp::missClipsCnt = sizeof(GoalfinderApp::missClips) / sizeof(GoalfinderApp::missClips[0]);

// FreeRTOS Handles
TaskHandle_t GoalfinderApp::TaskAudioHandle = nullptr;
TaskHandle_t GoalfinderApp::TaskDetectionHandle = nullptr;
TaskHandle_t GoalfinderApp::TaskLedHandle = nullptr;
TaskHandle_t GoalfinderApp::TaskLoggerHandle = nullptr;
SemaphoreHandle_t GoalfinderApp::xMutex = nullptr;

// Constructor
GoalfinderApp::GoalfinderApp() :
    Singleton<GoalfinderApp>(),
    fileSystem(true),
    webServer(&fileSystem),
    sntp(),
    audioPlayer(&fileSystem, pinI2sBclk, pinI2sWclk, pinI2sDataOut),
    tofSensor(),
    vibrationSensor(),
    ledController(pinLedPwm, ledPwmChannel),
    announcing(false),
    metronomeIntervalMs(2000),
    lastMetronomeTickTime(0),
    announcement(Announcement::None),
    lastShockTime(0),
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
    // initialize queue and task for asynchronous logging
    Logger::begin(115200);

    randomSeed(analogRead(pinRandomSeed));

    if (fileSystem.Begin()) {
        WiFiSetup();
        delay(200); // Allow SoftAP setup

        dnsServer.stop();
        dnsServer.start(53, "*", WiFi.softAPIP());
        Logger::log("GoalfinderApp", Logger::LogLevel::INFO, "DNS server started for captive portal");

        webServer.Begin();
        sntp.Init();
        vibrationSensor.Init();
        tofSensor.Init(pinTofScl, pinTofSda);
        ledController.SetMode(LedMode::Flash);

        UpdateSettings(true);

        xMutex = xSemaphoreCreateMutex();    

        xTaskCreatePinnedToCore(TaskAudio, "Audio", 8192, this, 2, &TaskAudioHandle, 1);
        xTaskCreatePinnedToCore(TaskDetection, "Detection", 8192, this, 2, &TaskDetectionHandle, 0);
        xTaskCreatePinnedToCore(TaskLed, "LED", 8192, this, 2, &TaskLedHandle, 0);
        xTaskCreatePinnedToCore(TaskLogger, "Logger", 4096, this, 1, &TaskLoggerHandle, 0);

        Logger::log("GoalfinderApp", Logger::LogLevel::OK, "All tasks started");
    } else {
        Logger::log("GoalfinderApp", Logger::LogLevel::ERROR, "FS initialization failed");
    }
}

void GoalfinderApp::WiFiSetup() {
    Settings* settings = Settings::GetInstance();

    if (settings->IsFirstRun()) {
        ApplyDeviceNameByScan();
    }

    String ssid = settings->GetDeviceName();
    String wifiPassword = settings->GetWifiPassword();

    WiFi.mode(WIFI_AP);

    if (wifiPassword.isEmpty()) {
        WiFi.softAP(ssid);
    } else {
        WiFi.softAP(ssid, wifiPassword.c_str());
    }
    WiFi.setSleep(false);
    Logger::log("GoalfinderApp", Logger::LogLevel::INFO, "SoftAP IP: %s", WiFi.softAPIP().toString().c_str());
}

void GoalfinderApp::ApplyDeviceNameByScan() {
    Settings* settings = Settings::GetInstance();

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    int n = WiFi.scanNetworks();
    Logger::log("GoalfinderApp", Logger::LogLevel::INFO, "First run found %d networks", n);

    bool usedNumbers[100] = { false };

    for (int i = 0; i < n; i++) {
        String ssid = WiFi.SSID(i);
        if (ssid.startsWith("GoalFinder")) {
            String numStr = ssid.substring(11);
            int num = numStr.toInt();
            if (num > 0 && num < 100) {
                usedNumbers[num] = true;
                Logger::log("GoalfinderApp", Logger::LogLevel::INFO, "Found existing device: %s (number %d)", ssid.c_str(), num);
            }
        }
    }
    WiFi.scanDelete();

    int nextNumber = 1;
    for (int i = 1; i < 100; i++) {
        if (!usedNumbers[i]) {
            nextNumber = i;
            break;
        }
    }

    char numberStr[3];
    snprintf(numberStr, sizeof(numberStr), "%02d", nextNumber);
    String deviceName = "GoalFinder " + String(numberStr);
    
    settings->SetDeviceName(deviceName);
    settings->SetFirstRun(false);
    Logger::log("GoalfinderApp", Logger::LogLevel::OK, "First run assigned device name '%s'", deviceName.c_str());
}

void GoalfinderApp::UpdateSettings(bool force) {
    Settings* settings = Settings::GetInstance();
    if (force || settings->IsModified()) {
        audioPlayer.SetVolume(settings->GetVolume());
        ledController.SetMode(settings->GetLedMode());
        vibrationSensor.SetSensitivity(settings->GetVibrationSensorSensitivity());
        distanceOnlyHitDetection = settings->GetDistanceOnlyHitDetection();
        afterHitTimeoutMs = settings->GetAfterHitTimeout() * 1000UL;
    }
}

// Tasks
void GoalfinderApp::TaskAudio(void *pvParameters) {
    GoalfinderApp* app = (GoalfinderApp*)pvParameters;
    while (app->loop) {
        if (app->IsSoundEnabled()) {
            if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
                app->audioPlayer.Loop();
                bool isPlaying = app->audioPlayer.IsPlaying();
                xSemaphoreGive(xMutex);
                if (!isPlaying) {
                    app->TickMetronome();
                }
            }
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

void GoalfinderApp::TaskDetection(void *pvParameters) {
    GoalfinderApp* app = (GoalfinderApp*)pvParameters;
    while (app->loop) {
        app->UpdateSettings();
        app->DetectShot();
        app->ProcessAnnouncement();
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

void GoalfinderApp::TaskLed(void *pvParameters) {
    GoalfinderApp* app = (GoalfinderApp*)pvParameters;
    while (app->loop) {
        app->ledController.Loop();
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

void GoalfinderApp::TaskLogger(void *pvParameters) {
    GoalfinderApp* app = (GoalfinderApp*)pvParameters;
    while (app->loop) {
        Logger::Loop();
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

// Play metronome sound
void GoalfinderApp::TickMetronome() {
    unsigned long currentTime = millis();
    if ((currentTime - lastMetronomeTickTime) > metronomeIntervalMs) {
        lastMetronomeTickTime = currentTime;
        const char* clipName = (lastShockTime > 0) ? waitingClip : tickClips[Settings::GetInstance()->GetMetronomeSound()];
        PlaySound(clipName);
    }
}

void GoalfinderApp::DetectShot() {
    if (!(lastHitTime > 0 && (millis() - lastHitTime) < afterHitTimeoutMs)) {
        int distance = tofSensor.ReadSingleMillimeters();
        Logger::log("Detection", Logger::LogLevel::DEBUG, "Distance: %f", distance);
        if (distanceOnlyHitDetection) {
            if (!(announcing && audioPlayer.IsPlaying())) {
                announcing = false;
                if (distance > 20 && distance < Settings::GetInstance()->GetBallHitDetectionDistance()) {
                    AnnounceHit();
                    lastHitTime = millis();
                }
            }
        } else {
            if (lastShockTime == 0) {
                if (!(announcing && audioPlayer.IsPlaying())) {
                    announcing = false;
                    long vibration = vibrationSensor.Vibration(10000);
                    if (vibration > shotVibrationThreshold) {
                        lastShockTime = millis();
                        Logger::log("GoalfinderApp", Logger::LogLevel::INFO, "Shot detected");
                    }
                }
            }

            unsigned long currentTime = millis();

            if (lastShockTime > 0 && (currentTime - lastShockTime) < maxShotDurationMs) {
                int distance = tofSensor.ReadSingleMillimeters();
                if (distance > 20 && distance < Settings::GetInstance()->GetBallHitDetectionDistance()) {
                    AnnounceHit();
                    lastShockTime = 0;
                    lastHitTime = millis();
                }
            }

            if (lastShockTime > 0 && (currentTime - lastShockTime) > maxShotDurationMs) {
                AnnounceMiss();
                lastShockTime = 0;
                lastHitTime = millis();
            }
        }
    }
}

void GoalfinderApp::ProcessAnnouncement() {
    switch (announcement) {
        case Announcement::Shot:
            AnnounceEvent("-> shot", nullptr);
            break;
        case Announcement::Hit:
            AnnounceEvent("-> hit", hitClips[Settings::GetInstance()->GetHitSound()]);
            break;
        case Announcement::Miss:
            AnnounceEvent("-> miss", missClips[Settings::GetInstance()->GetHitSound()]);
            break;
        default:
            break;
    }
    announcement = Announcement::None;
}

// Old comment mentioned some missing logic here but works fine
void GoalfinderApp::AnnounceHit() {
    detectedHits++;
    announcement = Announcement::Hit;
    Logger::log("GoalfinderApp", Logger::LogLevel::OK, "Hit detected (total hits: %d)", detectedHits);
}

void GoalfinderApp::AnnounceMiss() {
    detectedMisses++;
    announcement = Announcement::Miss;
    Logger::log("GoalfinderApp", Logger::LogLevel::WARN, "Miss detected (total misses: %d)", detectedMisses);
}

void GoalfinderApp::AnnounceEvent(const char* traceMsg, const char* sound) {
    Logger::log("GoalfinderApp", Logger::LogLevel::INFO, "Announcing event '%s'", traceMsg);
    if (sound) {
        announcing = true;
        PlaySound(sound);
    }
}

void GoalfinderApp::PlaySound(const char* soundFileName) {
    if (soundFileName) {
        Logger::log("GoalfinderApp", Logger::LogLevel::INFO, "Starting playback '%s'", soundFileName);
        if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
            audioPlayer.PlayMP3(soundFileName);
            xSemaphoreGive(xMutex);
        }
    }
}

void GoalfinderApp::Process() {
    if (WiFi.softAPgetStationNum() >= 0) {
        dnsServer.processNextRequest();
    }
    delay(1);
}

int GoalfinderApp::GetDetectedHits()
{
    return detectedHits;
}

int GoalfinderApp::GetDetectedMisses()
{
    return detectedMisses;
}

void GoalfinderApp::ResetDetectedHits()
{
    detectedHits = 0;
}

void GoalfinderApp::ResetDetectedMisses()
{
    detectedMisses = 0;
}