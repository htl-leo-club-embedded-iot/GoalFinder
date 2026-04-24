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

#include "util/Logger.h"
#include <AudioPlayer.h>

/**
 * @file AudioPlayer.cpp
 * @brief Implementation of the AudioPlayer class declared in AudioPlayer.h.
 *
 * Handles object creation, MP3 playback control, and volume calculation.
 */

/**
 * @brief Construct and initialize audio sub‑objects.
 */
AudioPlayer::AudioPlayer(FileSystem *fileSystem, int bclkPin, int wclkPin, int doutPin) : volumePc(0) {
    this->fileSystem = fileSystem;
    fsSource = new AudioFileSourceFS(*fileSystem->GetInternalFileSystem());
    bufferedSource = nullptr;
    mp3Generator = new AudioGeneratorMP3();
    audioOutput = new AudioOutputI2S();
    audioOutput->SetPinout(bclkPin, wclkPin, doutPin);
    SetVolume(50);
}

/**
 * @brief Destructor stops playback and frees allocated objects.
 */
AudioPlayer::~AudioPlayer() {
    Stop();
    delete fsSource;
    delete mp3Generator;
    delete audioOutput;
}

/**
 * @brief Begin playback of an MP3 file.
 */
void AudioPlayer::PlayMP3(const char *path) {
    Stop();
    fsSource->open(path);
    bufferedSource = new AudioFileSourceBuffer(fsSource, AUDIO_BUFFER_SIZE);

    delay(AUDIO_BUFFER_TIMEOUT);

    mp3Generator->begin(bufferedSource, audioOutput);
}

/**
 * @brief Change playback volume.
 *
 * Converts a 0–100 percent value into a gain value suitable for the audio
 * output object and applies it. Logs the computed gain.
 */
void AudioPlayer::SetVolume(uint8_t percent) {
    if (percent != volumePc) {
        volumePc = percent;
        // gain is in range from 0.0 to 4.0-epsilon (4.0 mutes the sound)
        // "percent" is in range from 0 to 100
        float base = 25.0f;
        float epsilon = 0.01f;
        // clip the volume
        uint8_t gainPc = volumePc > 100 ? 100 : volumePc;
        // calculate the gain for the player
        float gain = (gainPc / base) - epsilon;

        Logger::Log("AudioPlayer", Logger::LogLevel::INFO, "%4.3f: setting audio gain to '%.3f'", millis() / 1000.0,
                    gain);
        audioOutput->SetGain(gain);
    }
}

/**
 * @brief Drive the MP3 generator; should be called from the main loop.
 */
void AudioPlayer::Loop() {
    if (mp3Generator->isRunning() && !mp3Generator->loop()) {
        mp3Generator->stop();
    }
}

/**
 * @brief Stop current playback and free the buffer.
 */
void AudioPlayer::Stop() {
    if (IsPlaying()) {
        mp3Generator->stop();
    }
    if (bufferedSource) {
        delete bufferedSource;
        bufferedSource = nullptr;
    }
}

/**
 * @brief Query playback state.
 */
bool AudioPlayer::IsPlaying() { return mp3Generator->isRunning(); }