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

#include "FileSystem.h"
#include "util/Logger.h"

FileSystem::FileSystem(bool deleteOnFailed) { this->deleteOnFailed = deleteOnFailed; };

bool FileSystem::Begin() { return LittleFS.begin(deleteOnFailed); }

File FileSystem::OpenFile(String path) {
    Logger::log("FileSystem", Logger::LogLevel::INFO, "Opened file: %s", path.c_str());
    return LittleFS.open(path, FILE_READ);
}

bool FileSystem::FileExists(String path) { return LittleFS.exists(path); }

int FileSystem::GetFreeSpace() { return 0; }

fs::FS *FileSystem::GetInternalFileSystem() { return &LittleFS; }

FileSystem::~FileSystem() {}