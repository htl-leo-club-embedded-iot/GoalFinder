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

#ifndef ___DNS_SERVER_H
#define ___DNS_SERVER_H

#include <Arduino.h>
#include <WiFiUdp.h>

class GFDNSServer {
public:
    GFDNSServer();
    ~GFDNSServer();

    /**
     * Starts the UDP socket on port 53 and sets the IP address to return for
     * `goalfinder.local` A-record queries.
     */
    void Begin(const IPAddress& resolvedIP);

    /** Processes one pending packet (non-blocking). Call repeatedly from a loop. */
    void Loop();

    /**
     * FreeRTOS task entry point.
     * @param pvParameters  Pointer to a GFDNSServer instance.
     */
    static void Task(void* pvParameters);

private:
    static const uint16_t DNS_PORT      = 53;
    static const uint32_t DNS_TTL       = 60;    // seconds
    static const size_t   DNS_BUF_SIZE  = 512;

    /** The single hostname this server resolves (lower-case). */
    static const char* HOSTNAME;

    WiFiUDP   _udp;
    IPAddress _resolvedIP;

    /** Reads and handles the current UDP packet. */
    void HandlePacket();

    /**
     * Parses a DNS name from @p buf starting at @p offset.
     * @param[out] name       Dot-separated hostname (lower-case)
     * @param[out] endOffset  Byte offset directly after the name field
     * @return true on success, false if the buffer is malformed
     */
    bool ParseName(const uint8_t* buf, size_t bufLen,
                   uint16_t offset, String& name, uint16_t& endOffset);
};

#endif