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

#include "SoftwareUpdater.h"
#include <Update.h>

// ── static member initialization ─────────────────────────────────────────────
SoftwareUpdater::UpdatePhase SoftwareUpdater::phase              = PHASE_IDLE;
uint32_t                     SoftwareUpdater::firmwareSize        = 0;
uint32_t                     SoftwareUpdater::filesystemSize      = 0;
uint32_t                     SoftwareUpdater::firmwareBytesWritten = 0;
uint32_t                     SoftwareUpdater::fsBytesWritten      = 0;
uint8_t                      SoftwareUpdater::headerBuffer[GFPKG_HEADER_SIZE] = {};
uint8_t                      SoftwareUpdater::headerPos           = 0;

// ── constructor ──────────────────────────────────────────────────────────────
SoftwareUpdater::SoftwareUpdater(AsyncWebServer* server) {
    this->server = server;
}

// ── helpers ──────────────────────────────────────────────────────────────────
void SoftwareUpdater::ResetState() {
    phase              = PHASE_HEADER;
    firmwareSize       = 0;
    filesystemSize     = 0;
    firmwareBytesWritten = 0;
    fsBytesWritten     = 0;
    headerPos          = 0;
    memset(headerBuffer, 0, GFPKG_HEADER_SIZE);
}

// ── register the OTA endpoint ────────────────────────────────────────────────
void SoftwareUpdater::Begin(const char* uri) {
    server->on(uri, HTTP_POST, [](AsyncWebServerRequest *request) {
        bool success = (phase == PHASE_COMPLETE) && !Update.hasError();
        AsyncWebServerResponse* response = request->beginResponse(
            200, "text/plain", success ? "OK" : "FAIL");
        response->addHeader("Connection", "close");
        request->send(response);
        phase = PHASE_IDLE;
        delay(500); // Allow response to be sent before restarting
        ESP.restart();
    }, HandleUpdate);
}

// ── chunked upload handler (called for every chunk of the upload) ────────────
//
// Supports two formats:
//   1. Combined .gfpkg  – 16-byte header + firmware + filesystem
//   2. Legacy   .bin    – plain firmware binary (auto-detected when magic
//                         bytes do not match "GFPK")
//
void SoftwareUpdater::HandleUpdate(AsyncWebServerRequest *request,
                                   String filename, size_t index,
                                   uint8_t *data, size_t len, bool final) {
    // ── first chunk: reset state machine ─────────────────────────────────────
    if (!index) {
        Serial.println("[INFO][SoftwareUpdater.cpp] Update started");
        ResetState();
    }

    if (phase == PHASE_ERROR || phase == PHASE_COMPLETE) return;

    size_t offset = 0;   // position within the current chunk

    while (offset < len) {
        switch (phase) {

        // ── 1. Buffer the 16-byte header ─────────────────────────────────────
        case PHASE_HEADER: {
            while (offset < len && headerPos < GFPKG_HEADER_SIZE) {
                headerBuffer[headerPos++] = data[offset++];
            }

            if (headerPos < GFPKG_HEADER_SIZE) {
                break; // need more data
            }

            // Header complete – decide format
            if (memcmp(headerBuffer, GFPKG_MAGIC, 4) == 0) {
                // ── combined .gfpkg package ──────────────────────────────────
                uint8_t version = headerBuffer[4];
                memcpy(&firmwareSize,    &headerBuffer[5], 4);
                memcpy(&filesystemSize,  &headerBuffer[9], 4);

                Serial.printf("[INFO][SoftwareUpdater.cpp] GFPKG v%u: "
                              "firmware=%u bytes, filesystem=%u bytes\n",
                              version, firmwareSize, filesystemSize);

                if (firmwareSize > 0) {
                    if (!Update.begin(firmwareSize, U_FLASH)) {
                        Update.printError(Serial);
                        phase = PHASE_ERROR;
                        return;
                    }
                    phase = PHASE_FIRMWARE;
                } else if (filesystemSize > 0) {
                    // Filesystem-only package (no firmware)
                    if (!Update.begin(filesystemSize, U_SPIFFS)) {
                        Update.printError(Serial);
                        phase = PHASE_ERROR;
                        return;
                    }
                    phase = PHASE_FILESYSTEM;
                } else {
                    Serial.println("[ERROR][SoftwareUpdater.cpp] GFPKG has zero-size payload");
                    phase = PHASE_ERROR;
                    return;
                }
            } else {
                // ── legacy plain .bin firmware ───────────────────────────────
                Serial.println("[INFO][SoftwareUpdater.cpp] Legacy firmware update detected");
                int contentLen = request->contentLength();
                if (!Update.begin(contentLen, U_FLASH)) {
                    Update.printError(Serial);
                    phase = PHASE_ERROR;
                    return;
                }
                // The 16 bytes we buffered are actually firmware data – write them
                if (Update.write(headerBuffer, GFPKG_HEADER_SIZE) != GFPKG_HEADER_SIZE) {
                    Update.printError(Serial);
                    phase = PHASE_ERROR;
                    return;
                }
                phase = PHASE_LEGACY;
            }
            break;
        }

        // ── 2. Write firmware partition ──────────────────────────────────────
        case PHASE_FIRMWARE: {
            uint32_t remaining = firmwareSize - firmwareBytesWritten;
            size_t   available = len - offset;
            size_t   toWrite   = (available < remaining) ? available : remaining;

            if (Update.write(data + offset, toWrite) != toWrite) {
                Update.printError(Serial);
                phase = PHASE_ERROR;
                return;
            }

            firmwareBytesWritten += toWrite;
            offset               += toWrite;

            if (firmwareBytesWritten >= firmwareSize) {
                if (!Update.end(true)) {
                    Update.printError(Serial);
                    phase = PHASE_ERROR;
                    return;
                }
                Serial.println("[OK][SoftwareUpdater.cpp] Firmware update complete");

                if (filesystemSize > 0) {
                    Serial.printf("[INFO][SoftwareUpdater.cpp] Starting filesystem "
                                  "update: %u bytes\n", filesystemSize);
                    if (!Update.begin(filesystemSize, U_SPIFFS)) {
                        Update.printError(Serial);
                        phase = PHASE_ERROR;
                        return;
                    }
                    phase = PHASE_FILESYSTEM;
                } else {
                    phase = PHASE_COMPLETE;
                }
            }
            break;
        }

        // ── 3. Write filesystem (LittleFS) partition ─────────────────────────
        case PHASE_FILESYSTEM: {
            size_t toWrite = len - offset;

            if (Update.write(data + offset, toWrite) != toWrite) {
                Update.printError(Serial);
                phase = PHASE_ERROR;
                return;
            }

            fsBytesWritten += toWrite;
            offset          += toWrite;
            break;
        }

        // ── 4. Legacy plain firmware .bin ────────────────────────────────────
        case PHASE_LEGACY: {
            size_t toWrite = len - offset;

            if (Update.write(data + offset, toWrite) != toWrite) {
                Update.printError(Serial);
                phase = PHASE_ERROR;
                return;
            }
            offset += toWrite;
            break;
        }

        default:
            return;
        }
    }

    // ── last chunk: finalise whichever update is active ──────────────────────
    if (final) {
        if (phase == PHASE_FIRMWARE || phase == PHASE_FILESYSTEM || phase == PHASE_LEGACY) {
            if (!Update.end(true)) {
                Update.printError(Serial);
                phase = PHASE_ERROR;
            } else {
                Serial.println("[OK][SoftwareUpdater.cpp] Update complete");
                phase = PHASE_COMPLETE;
            }
        }
    }
}

bool SoftwareUpdater::IsUpdating() {
    return Update.isRunning();
}