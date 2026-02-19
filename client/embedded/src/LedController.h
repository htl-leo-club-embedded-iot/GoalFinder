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
#include "LedMode.h"
#include "Settings.h"
#include <stdint.h>


class LedController
{
    private:    
        void RenderPermanentStep(uint8_t brightness);
        void RenderFadeStep();
        void RenderFlashStep();
        void RenderTurboStep();
        uint8_t ScaleBrightness(uint8_t value);

        int channel;
        LedMode mode;
        uint64_t lastStepTimeMs;

    public:
        LedController(int ledPin, int ledChannel);
        ~LedController();
        void Loop();
        void SetMode(LedMode mode);
        LedMode GetMode();
};