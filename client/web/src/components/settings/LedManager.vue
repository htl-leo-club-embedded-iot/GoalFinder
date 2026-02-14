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

<script setup lang="ts">
import {useSettingsStore} from "@/stores/settings";
import Button from "@/components/Button.vue";

const settings = useSettingsStore();

function setLedMode(value: string, mode: number) {
  settings.ledModeStr = value;
  settings.ledMode = mode;
}
</script>

<template>
  <div class="container" style="padding-top: 0;">
    <div id="led">
      <div class="label-container">
        <h3 for="modus">{{ $t("word.led_mode") }}</h3>
        <div class="button-container">
          <Button class="button" @click="setLedMode('Ein', 1)">{{ $t("word.on") }}</Button>
          <Button class="button" @click="setLedMode('Fade', 2)">{{ $t("word.fade") }}</Button>
          <Button class="button" @click="setLedMode('Blitzartig', 3)">{{ $t("word.flash") }}</Button>
          <Button class="button" @click="setLedMode('Turbo', 4)">{{ $t("word.turbo") }}</Button>
          <Button class="button" @click="setLedMode('Aus', 5)">{{ $t("word.off") }}</Button>
        </div>
      </div>
    </div>
    <div class="current-mode">
      <p style="margin: 0;">{{ $t("word.curr_mode") }}: {{ settings.ledModeStr }}</p>
    </div>
  </div>
</template>