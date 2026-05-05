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
#include <AudioFileSourceBuffer.h>
#include <AudioFileSourceFS.h>
#include <AudioGeneratorMP3.h>
#include <AudioOutputI2S.h>
#include <FileSystem.h>

#define AUDIO_BUFFER_TIMEOUT 100

/**
 * @file AudioPlayer.h
 * @brief Simple MP3 playback wrapper for the ESP audio library.
 *
 * The AudioPlayer class encapsulates the various objects required to read an
 * MP3 file from the filesystem and stream it through I2S. It also provides
 * basic volume control and a polling loop to drive playback.
 */
class AudioPlayer {
  public:
    /**
     * @brief Create a new AudioPlayer instance.
     *
     * @param fileSystem Pointer to the project's FileSystem helper.
     * @param bclkPin    I2S bit clock pin.
     * @param wclkPin    I2S word clock pin.
     * @param doutPin    I2S data output pin.
     */
    AudioPlayer(FileSystem *fileSystem, int bclkPin, int wclkPin, int doutPin);

    /**
     * @brief Clean up allocated audio objects.
     */
    virtual ~AudioPlayer();

    /**
     * @brief Start playing the specified MP3 file.
     *
     * Any previous playback is stopped first. The file is buffered to reduce
     * underruns.
     *
     * @param path Path to the MP3 file on the filesystem.
     */
    void PlayMP3(const char *path);

    /**
     * @brief Adjust the output volume.
     *
     * @param percent Volume level from 0 (silent) to 100 (maximum).
     */
    void SetVolume(uint8_t percent);

    /**
     * @brief Polling function that must be called regularly.
     *
     * When playback is active this will drive the underlying AudioGenerator
     * until the track finishes, at which point it stops itself.
     */
    void Loop();

    /**
     * @brief Stop playback and release the buffered source.
     */
    void Stop();

    /**
     * @brief Query whether audio is currently playing.
     *
     * @return true if an MP3 is being decoded and sent to the output.
     */
    bool IsPlaying();

  private:
    static const int AUDIO_BUFFER_SIZE = 2048;
    FileSystem *fileSystem;
    AudioFileSourceFS *fsSource;
    AudioFileSourceBuffer *bufferedSource;
    AudioGeneratorMP3 *mp3Generator;
    AudioOutputI2S *audioOutput;
    uint8_t volumePc;
};