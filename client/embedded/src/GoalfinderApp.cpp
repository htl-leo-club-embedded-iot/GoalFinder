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

// Hardware pins and constants
const int GoalfinderApp::pinTofSda = 22;
const int GoalfinderApp::pinTofScl = 21;
const int GoalfinderApp::pinI2sBclk = 23;
const int GoalfinderApp::pinI2sWclk = 5;
const int GoalfinderApp::pinI2sDataOut = 19;
const int GoalfinderApp::pinLedPwm = 17;
const int GoalfinderApp::pinRandomSeed = 36;

const int GoalfinderApp::ledPwmChannel = 0;

const int GoalfinderApp::ballHitDetectionDistance = 180;
const int GoalfinderApp::shotVibrationThreshold = 2000;
const int GoalfinderApp::maxShotDurationMs = 5000;

const char* GoalfinderApp::waitingClip = "/waiting.mp3";

const char* GoalfinderApp::hitClips[] = { "/hit-1.mp3" };
const int   GoalfinderApp::hitClipsCnt = sizeof(GoalfinderApp::hitClips) / sizeof(GoalfinderApp::hitClips[0]);

const char* GoalfinderApp::tickClips[] = { "/tick-1.mp3" };
const int   GoalfinderApp::tickClipsCnt = sizeof(GoalfinderApp::tickClips) / sizeof(GoalfinderApp::tickClips[0]);

const char* GoalfinderApp::missClips[] = { "/miss-1.mp3" };
const int   GoalfinderApp::missClipsCnt = sizeof(GoalfinderApp::missClips) / sizeof(GoalfinderApp::missClips[0]);

// FreeRTOS Handles
TaskHandle_t GoalfinderApp::TaskAudio = nullptr;
TaskHandle_t GoalfinderApp::TaskDetection = nullptr;
TaskHandle_t GoalfinderApp::TaskLed = nullptr;
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
    isSoundEnabled(true)
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
    randomSeed(analogRead(pinRandomSeed));

    if (!fileSystem.Begin()) {
        Serial.println("FS initialization failed");
        return;
    }

    // Start WiFi
    WiFiSetup();

    // Start DNS server for captive portal (redirect all domains to AP IP)
    dnsServer.start(53, "*", WiFi.softAPIP());
    Serial.println("[INFO] DNS server started for captive portal");

    webServer.Begin();
    sntp.Init();
    vibrationSensor.Init();
    tofSensor.Init(pinTofScl, pinTofSda);
    ledController.SetMode(LedMode::Flash);

    UpdateSettings(true);

    xMutex = xSemaphoreCreateMutex();

    xTaskCreatePinnedToCore(TaskAudioCode, "Audio", 8192, this, 2, &TaskAudio, 1);
    xTaskCreatePinnedToCore(TaskDetectionCode, "Detection", 8192, this, 2, &TaskDetection, 0);
    xTaskCreatePinnedToCore(TaskLedCode, "LED", 8192, this, 2, &TaskLed, 0);

    Serial.println("All tasks started.");
}

void GoalfinderApp::WiFiSetup() {
    Settings* settings = Settings::GetInstance();

    if (settings->IsFirstRun()) {
        WiFi.mode(WIFI_STA);
        WiFi.disconnect();
        delay(100);

        int n = WiFi.scanNetworks();
        Serial.printf("[INFO] First run: found %d networks\n", n);

        bool usedNumbers[100] = { false };

        for (int i = 0; i < n; i++) {
            String ssid = WiFi.SSID(i);
            if (ssid.startsWith("GoalFinder")) {
                String numStr = ssid.substring(11);
                int num = numStr.toInt();
                if (num > 0 && num < 100) {
                    usedNumbers[num] = true;
                    Serial.printf("[INFO]   Found existing device: %s (number %d)\n", ssid.c_str(), num);
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
        Serial.printf("[INFO] First run: assigned device name '%s'\n", deviceName.c_str());
    }

    String ssid = settings->GetDeviceName();
    String wifiPw = settings->GetDevicePassword();

    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, wifiPw);
    WiFi.setSleep(false);
    Serial.println(WiFi.softAPIP());
}

void GoalfinderApp::UpdateSettings(bool force) {
    Settings* settings = Settings::GetInstance();
    if (force || settings->IsModified()) {
        audioPlayer.SetVolume(settings->GetVolume());
        ledController.SetMode(settings->GetLedMode());
        vibrationSensor.SetSensitivity(settings->GetVibrationSensorSensitivity());
    }
}

// Tasks
void GoalfinderApp::TaskAudioCode(void *pvParameters) {
    GoalfinderApp* app = (GoalfinderApp*)pvParameters;
    for (;;) {
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

void GoalfinderApp::TaskDetectionCode(void *pvParameters) {
    // Try and remove endless loop by adding a exit option
    GoalfinderApp* app = (GoalfinderApp*)pvParameters;
    for (;;) {
        app->UpdateSettings();
        app->DetectShot();
        app->ProcessAnnouncement();
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

void GoalfinderApp::TaskLedCode(void *pvParameters) {
    GoalfinderApp* app = (GoalfinderApp*)pvParameters;
    // Try and remove endless loop by adding a exit option
    for (;;) {
        app->ledController.Loop();
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

// Play metronome sound
void GoalfinderApp::TickMetronome() {
    unsigned long currentTime = millis();
    if ((currentTime - lastMetronomeTickTime) > metronomeIntervalMs) {
        lastMetronomeTickTime = currentTime;
        const char* clipName = (lastShockTime > 0) ? waitingClip : tickClips[0];
        PlaySound(clipName);
    }
}

void GoalfinderApp::DetectShot() {
    if (lastShockTime == 0) {
        if (!(announcing && audioPlayer.IsPlaying())) {
            announcing = false;
            long vibration = vibrationSensor.Vibration(10000);
            if (vibration > shotVibrationThreshold) {
                lastShockTime = millis();
                Serial.printf("%4.3f: shot detected\n", millis() / 1000.0);
            }
        }
    }

    unsigned long currentTime = millis();
    if (lastShockTime > 0 && (currentTime - lastShockTime) < maxShotDurationMs) {
        int distance = tofSensor.ReadSingleMillimeters();
        if (distance > 20 && distance < ballHitDetectionDistance) {
            AnnounceHit();
            lastShockTime = 0;
        }
    }
    if (lastShockTime > 0 && (currentTime - lastShockTime) > maxShotDurationMs) {
        AnnounceMiss();
        lastShockTime = 0;
    }
}

void GoalfinderApp::ProcessAnnouncement() {
    switch (announcement) {
        case Announcement::Shot:
            AnnounceEvent("-> shot", nullptr);
            break;
        case Announcement::Hit:
            AnnounceEvent("-> hit", hitClips[0]);
            break;
        case Announcement::Miss:
            AnnounceEvent("-> miss", missClips[0]);
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
    Serial.printf("%4.3f: Hit detected! Total hits: %d\n", millis() / 1000.0, detectedHits);
}

void GoalfinderApp::AnnounceMiss() {
    detectedMisses++;
    announcement = Announcement::Miss;
    Serial.printf("%4.3f: Miss detected! Total misses: %d\n", millis() / 1000.0, detectedMisses);
}

void GoalfinderApp::AnnounceEvent(const char* traceMsg, const char* sound) {
    Serial.printf("%4.3f: announcing event '%s'\n", millis() / 1000.0, traceMsg);
    if (sound) {
        announcing = true;
        PlaySound(sound);
    }
}

void GoalfinderApp::PlaySound(const char* soundFileName) {
    if (soundFileName) {
        Serial.printf("%4.3f: starting playback of: '%s'\n", millis() / 1000.0, soundFileName);
        if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
            audioPlayer.PlayMP3(soundFileName);
            xSemaphoreGive(xMutex);
        }
    }
}

void GoalfinderApp::Process() {
    // Process DNS requests for captive portal
    dnsServer.processNextRequest();
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