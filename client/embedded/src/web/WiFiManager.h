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

#ifndef __WEB_WIFI_MANAGER_H
#define __WEB_WIFI_MANAGER_H

#include "..\Settings.h"
#include "..\util\Logger.h"
#include <WiFi.h>
#include <DNSServer.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

class WiFiManager {
    public:
        WiFiManager();

        /** Initializes WiFi - connects to external network or starts AP based on settings. */
        void Init();

        /** Periodic loop for WiFi management (DNS processing, reconnection). */
        void Loop();

        /** Returns true if operating in external network (STA) mode. */
        bool IsExternalNetwork() const;

    private:
        void SetupAccessPoint();
        void SetupExternalNetwork();
        void MonitorConnection();
        void ApplyDeviceNameByScan();

        DNSServer dnsServer;
        bool useExternalNW;
        bool connected;
        unsigned long lastReconnectAttemptMs;
        static const unsigned long reconnectIntervalMs;
        SemaphoreHandle_t wifiMutex;
};

#endif