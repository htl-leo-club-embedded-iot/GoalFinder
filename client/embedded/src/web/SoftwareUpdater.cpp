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
#include "Settings.h"
#include "util/Logger.h"

// static member initialization 
WebServer*                   SoftwareUpdater::_server              = nullptr;
SoftwareUpdater::UpdatePhase SoftwareUpdater::phase                = PHASE_IDLE;
uint32_t                     SoftwareUpdater::firmwareSize          = 0;
uint32_t                     SoftwareUpdater::filesystemSize        = 0;
uint32_t                     SoftwareUpdater::firmwareBytesWritten  = 0;
uint32_t                     SoftwareUpdater::fsBytesWritten        = 0;
uint8_t                      SoftwareUpdater::headerBuffer[GFPKG_HEADER_SIZE] = {};
uint8_t                      SoftwareUpdater::headerPos             = 0;

// constructor
SoftwareUpdater::SoftwareUpdater(WebServer* server) {
    _server = server;
}

// helpers
void SoftwareUpdater::ResetState() {
    phase                = PHASE_HEADER;
    firmwareSize         = 0;
    filesystemSize       = 0;
    firmwareBytesWritten = 0;
    fsBytesWritten       = 0;
    headerPos            = 0;
    memset(headerBuffer, 0, GFPKG_HEADER_SIZE);
}

// Register the OTA endpoint on the synchronous WebServer
void SoftwareUpdater::Begin(const char* uri) {
    _server->on(uri, HTTP_POST, HandleComplete, HandleUpload);
}

// Called once the full upload has been received
void SoftwareUpdater::HandleComplete() {
    bool success = (phase == PHASE_COMPLETE) && !Update.hasError();
    if (success) {
        Settings::GetInstance()->SetUpdateSuccess(true);
    }

    _server->sendHeader("Access-Control-Allow-Origin", "*");
    _server->sendHeader("Connection", "close");
    _server->send(200, "text/plain", success ? "OK" : "FAIL");

    phase = PHASE_IDLE;
    delay(500);
    ESP.restart();
}

// Called for every chunk of the file upload
void SoftwareUpdater::HandleUpload() {
    HTTPUpload& upload = _server->upload();

    if (upload.status == UPLOAD_FILE_START) {
        Logger::log("SoftwareUpdater", Logger::LogLevel::INFO, "Update started: %s", upload.filename.c_str());
        ResetState();
    } else if (upload.status == UPLOAD_FILE_WRITE) {
        ProcessChunk(upload.buf, upload.currentSize, false);
    } else if (upload.status == UPLOAD_FILE_END) {
        ProcessChunk(nullptr, 0, true);
    }
}

// Unified chunk processor – same state-machine logic as before
//
// Supports two formats:
//   1. Combined .gfpkg  – 16-byte header + firmware + filesystem
//   2. Legacy   .bin    – plain firmware binary (auto-detected when magic
//                         bytes do not match "GFPK")
//
void SoftwareUpdater::ProcessChunk(uint8_t* data, size_t len, bool final) {
    if (phase == PHASE_ERROR || phase == PHASE_COMPLETE) return;

    size_t offset = 0;

    while (!final && offset < len) {
        switch (phase) {

        // 1. Buffer the 16-byte header 
        case PHASE_HEADER: {
            while (offset < len && headerPos < GFPKG_HEADER_SIZE) {
                headerBuffer[headerPos++] = data[offset++];
            }

            if (headerPos < GFPKG_HEADER_SIZE) {
                break; // need more data
            }

            // Header complete – decide format
            if (memcmp(headerBuffer, GFPKG_MAGIC, 4) == 0) {
                uint8_t version = headerBuffer[4];
                memcpy(&firmwareSize,   &headerBuffer[5], 4);
                memcpy(&filesystemSize, &headerBuffer[9], 4);

                Logger::log("SoftwareUpdater", Logger::LogLevel::INFO,
                            "GFPKG v%u: firmware=%u bytes, filesystem=%u bytes",
                            version, firmwareSize, filesystemSize);

                if (firmwareSize > 0) {
                    if (!Update.begin(firmwareSize, U_FLASH)) {
                        Update.printError(Serial);
                        phase = PHASE_ERROR;
                        return;
                    }
                    phase = PHASE_FIRMWARE;
                } else if (filesystemSize > 0) {
                    if (!Update.begin(filesystemSize, U_SPIFFS)) {
                        Update.printError(Serial);
                        phase = PHASE_ERROR;
                        return;
                    }
                    phase = PHASE_FILESYSTEM;
                } else {
                    Logger::log("SoftwareUpdater", Logger::LogLevel::ERROR, "GFPKG has zero-size payload");
                    phase = PHASE_ERROR;
                    return;
                }
            } else {
                // Legacy plain .bin firmware – use UPDATE_SIZE_UNKNOWN
                Logger::log("SoftwareUpdater", Logger::LogLevel::INFO, "Legacy firmware update detected");
                if (!Update.begin(UPDATE_SIZE_UNKNOWN, U_FLASH)) {
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

        // 2. Write firmware partition 
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
                Logger::log("SoftwareUpdater", Logger::LogLevel::OK, "Firmware upload complete");

                if (filesystemSize > 0) {
                    Logger::log("SoftwareUpdater", Logger::LogLevel::INFO,
                                "Starting filesystem update: %u bytes", filesystemSize);
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

        // 3. Write filesystem (LittleFS) partition
        case PHASE_FILESYSTEM: {
            size_t toWrite = len - offset;
            if (Update.write(data + offset, toWrite) != toWrite) {
                Update.printError(Serial);
                phase = PHASE_ERROR;
                return;
            }
            fsBytesWritten += toWrite;
            offset         += toWrite;
            break;
        }

        // 4. Legacy plain firmware .bin
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

    // Final chunk: finalize whichever update is active
    if (final) {
        if (phase == PHASE_FIRMWARE || phase == PHASE_FILESYSTEM || phase == PHASE_LEGACY) {
            if (!Update.end(true)) {
                Update.printError(Serial);
                phase = PHASE_ERROR;
            } else {
                Logger::log("SoftwareUpdater", Logger::LogLevel::OK, "Update complete");
                phase = PHASE_COMPLETE;
            }
        }
    }
}

bool SoftwareUpdater::IsUpdating() {
    return Update.isRunning();
}