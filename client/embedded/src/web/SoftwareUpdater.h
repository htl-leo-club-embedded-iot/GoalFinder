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
#include <ESPAsyncWebServer.h>

#define GFPKG_HEADER_SIZE 16
#define GFPKG_MAGIC       "GFPK"

class SoftwareUpdater 
{
    public:
        SoftwareUpdater(AsyncWebServer* server);
        void Begin(const char* uri);
        static bool IsUpdating();

    private:
        AsyncWebServer* server;

        /// Phases of a combined (.gfpkg) or legacy (.bin) OTA update.
        enum UpdatePhase {
            PHASE_IDLE,         ///< No update in progress
            PHASE_HEADER,       ///< Receiving the 16-byte header
            PHASE_FIRMWARE,     ///< Writing firmware (app) partition
            PHASE_FILESYSTEM,   ///< Writing filesystem (LittleFS) partition
            PHASE_LEGACY,       ///< Plain .bin firmware-only update
            PHASE_COMPLETE,     ///< All phases finished successfully
            PHASE_ERROR         ///< An error occurred
        };

        static UpdatePhase   phase;
        static uint32_t      firmwareSize;
        static uint32_t      filesystemSize;
        static uint32_t      firmwareBytesWritten;
        static uint32_t      fsBytesWritten;
        static uint8_t       headerBuffer[GFPKG_HEADER_SIZE];
        static uint8_t       headerPos;

        static void ResetState();
        static void HandleUpdate(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
};