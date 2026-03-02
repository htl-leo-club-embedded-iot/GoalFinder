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

#include "DNSServer.h"
#include "../util/Logger.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

const char* GFDNSServer::HOSTNAME = "goalfinder.local";

GFDNSServer::GFDNSServer() {}
GFDNSServer::~GFDNSServer() {}

void GFDNSServer::Begin(const IPAddress& resolvedIP) {
    _resolvedIP = resolvedIP;
    _udp.begin(DNS_PORT);
    Logger::log("DNSServer", Logger::LogLevel::OK,
                "Started – resolving %s -> %d.%d.%d.%d",
                HOSTNAME,
                _resolvedIP[0], _resolvedIP[1],
                _resolvedIP[2], _resolvedIP[3]);
}

void GFDNSServer::Loop() {
    if (_udp.parsePacket() > 0) {
        HandlePacket();
    }
}

void GFDNSServer::Task(void* pvParameters) {
    GFDNSServer* dns = static_cast<GFDNSServer*>(pvParameters);
    while (true) {
        dns->Loop();
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

void GFDNSServer::HandlePacket() {
    uint8_t buf[DNS_BUF_SIZE];
    int len = _udp.read(buf, sizeof(buf));
    if (len < 12) return; 

    IPAddress  remoteIP   = _udp.remoteIP();
    uint16_t   remotePort = _udp.remotePort();

    uint16_t txId   = ((uint16_t)buf[0] << 8) | buf[1];
    uint16_t flags  = ((uint16_t)buf[2] << 8) | buf[3];
    uint16_t qdCnt  = ((uint16_t)buf[4] << 8) | buf[5];

    if ((flags & 0x8000) != 0)   return; 
    if ((flags & 0x7800) != 0)   return;
    if (qdCnt == 0)              return;

    String   queryName;
    uint16_t nameEnd;
    if (!ParseName(buf, (size_t)len, 12, queryName, nameEnd)) return;

    if (nameEnd + 4 > (uint16_t)len) return;
    uint16_t qtype  = ((uint16_t)buf[nameEnd]     << 8) | buf[nameEnd + 1];
    uint16_t qclass = ((uint16_t)buf[nameEnd + 2] << 8) | buf[nameEnd + 3];

    queryName.toLowerCase();
    bool isOurHost  = (queryName == String(HOSTNAME));
    bool isTypeA    = (qtype  == 1 || qtype  == 255);
    bool isClassIN  = (qclass == 1 || qclass == 255);

    if (!isOurHost || !isTypeA || !isClassIN) return;

    uint8_t rsp[DNS_BUF_SIZE];
    int     rLen = 0;

    rsp[rLen++] = buf[0];
    rsp[rLen++] = buf[1];

    uint16_t rFlags = 0x8400;
    if (flags & 0x0100) rFlags |= 0x0100; // mirror RD
    rsp[rLen++] = (rFlags >> 8) & 0xFF;
    rsp[rLen++] =  rFlags       & 0xFF;

    rsp[rLen++] = 0x00; rsp[rLen++] = 0x01;
    rsp[rLen++] = 0x00; rsp[rLen++] = 0x01;
    rsp[rLen++] = 0x00; rsp[rLen++] = 0x00;
    rsp[rLen++] = 0x00; rsp[rLen++] = 0x00;

    size_t questionLen = (size_t)(nameEnd + 4) - 12;
    memcpy(rsp + rLen, buf + 12, questionLen);
    rLen += (int)questionLen;

    rsp[rLen++] = 0xC0; rsp[rLen++] = 0x0C;
    rsp[rLen++] = 0x00; rsp[rLen++] = 0x01;
    rsp[rLen++] = 0x00; rsp[rLen++] = 0x01;
    rsp[rLen++] = (DNS_TTL >> 24) & 0xFF;
    rsp[rLen++] = (DNS_TTL >> 16) & 0xFF;
    rsp[rLen++] = (DNS_TTL >>  8) & 0xFF;
    rsp[rLen++] =  DNS_TTL        & 0xFF;
    rsp[rLen++] = 0x00; rsp[rLen++] = 0x04;
    rsp[rLen++] = _resolvedIP[0];
    rsp[rLen++] = _resolvedIP[1];
    rsp[rLen++] = _resolvedIP[2];
    rsp[rLen++] = _resolvedIP[3];

    _udp.beginPacket(remoteIP, remotePort);
    _udp.write(rsp, rLen);
    _udp.endPacket();

    Logger::log("DNSServer", Logger::LogLevel::INFO,
                "Resolved %s -> %d.%d.%d.%d",
                queryName.c_str(),
                _resolvedIP[0], _resolvedIP[1],
                _resolvedIP[2], _resolvedIP[3]);
}

bool GFDNSServer::ParseName(const uint8_t* buf, size_t bufLen,
                          uint16_t offset, String& name, uint16_t& endOffset) {
    name = "";
    bool first = true;

    while (offset < (uint16_t)bufLen) {
        uint8_t labelLen = buf[offset];

        if (labelLen == 0) {
            endOffset = offset + 1;
            return true;
        }

        if ((labelLen & 0xC0) == 0xC0) {
            if (offset + 1 >= (uint16_t)bufLen) return false;
            uint16_t ptr = ((uint16_t)(labelLen & 0x3F) << 8) | buf[offset + 1];
            endOffset = offset + 2;
            String subName;
            uint16_t dummy;
            if (!ParseName(buf, bufLen, ptr, subName, dummy)) return false;
            if (!first) name += ".";
            name += subName;
            return true;
        }

        if (offset + 1 + labelLen > (uint16_t)bufLen) return false;
        if (!first) name += ".";
        for (uint8_t i = 0; i < labelLen; i++) {
            name += (char)buf[offset + 1 + i];
        }
        first = false;
        offset += 1 + labelLen;
    }
    return false;
}
