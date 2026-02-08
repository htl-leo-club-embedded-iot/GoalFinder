#include <GoalfinderApp.h>
#include <HardwareSerial.h>
#include <Settings.h>

// === Hardware-Pins und Konstanten ===
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

// === FreeRTOS Handles ===
TaskHandle_t GoalfinderApp::TaskAudio = nullptr;
TaskHandle_t GoalfinderApp::TaskDetection = nullptr;
TaskHandle_t GoalfinderApp::TaskLed = nullptr;
SemaphoreHandle_t GoalfinderApp::xMutex = nullptr;

// === Konstruktor / Destruktor ===
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

// === Getter / Setter ===
void GoalfinderApp::SetIsSoundEnabled(bool value) {
    isSoundEnabled = value;
}

bool GoalfinderApp::IsSoundEnabled() {
    return isSoundEnabled;
}

// === Initialisierung ===
void GoalfinderApp::Init() {
    delay(100);
    Serial.begin(115200);
    randomSeed(analogRead(pinRandomSeed));

    if (!fileSystem.Begin()) {
        Serial.println("FS initialization failed");
        return;
    }

    Settings* settings = Settings::GetInstance();
    String ssid = settings->GetDeviceName();
    String wifiPw = settings->GetDevicePassword();

    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, wifiPw);
    WiFi.setSleep(false);
    Serial.println(WiFi.softAPIP());

    webServer.Begin();
    sntp.Init();
    vibrationSensor.Init();
    tofSensor.Init(pinTofScl, pinTofSda);
    ledController.SetMode(LedMode::Flash);

    UpdateSettings(true);

    xMutex = xSemaphoreCreateMutex();

    xTaskCreatePinnedToCore(TaskAudioCode, "TaskAudio", 8192, this, 1, &TaskAudio, 1);
    xTaskCreatePinnedToCore(TaskDetectionCode, "TaskDetection", 8192, this, 2, &TaskDetection, 0);
    xTaskCreatePinnedToCore(TaskLedCode, "TaskLed", 8192, this, 1, &TaskLed, 0);

    Serial.println("All tasks started.");
}

void GoalfinderApp::UpdateSettings(bool force) {


    Settings* settings = Settings::GetInstance();
    if (force || settings->IsModified()) {
        audioPlayer.SetVolume(settings->GetVolume());
        ledController.SetMode(settings->GetLedMode());
        vibrationSensor.SetSensitivity(settings->GetVibrationSensorSensitivity());
    }
}

// === Tasks ===
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
    for (;;) {
        app->ledController.Loop();
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

// === Hauptlogik ===
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

// === Neue fehlende Implementierungen ===
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

// === Dummy-Methode für Loop-kompatibilität ===
void GoalfinderApp::Process() {
    // Hauptloop kann leer bleiben, wenn Tasks aktiv sind
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