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
  <div>
    <div id="led">
      <div class="label-container">
        <label for="modus">{{ $t("word.led_mode") }}</label>
        <div class="button-container">
          <Button class="button" @click="setLedMode('Ein', 1)">{{ $t("word.on") }}</Button>
          <Button class="button" @click="setLedMode('Fade', 2)">{{ $t("word.fade") }}</Button>
          <Button class="button" @click="setLedMode('Blitzartig', 3)">{{ $t("word.flash") }}</Button>
          <Button class="button" @click="setLedMode('Turbo', 4)">{{ $t("word.turbo") }}</Button>
          <Button class="button" @click="setLedMode('Aus', 5)">{{ $t("word.off") }}</Button>
        </div>
      </div>
    </div>
    <div class="current-modus">
      <span>{{ $t("word.curr_mode") }}: {{ settings.ledModeStr }}</span>
    </div>
  </div>
</template>

<style scoped>
.label-container {
  display: flex;
  flex-direction: column;
  align-items: flex-start;
  margin-bottom: 1rem;
  margin-top: 0.5rem;
}

.button-container {
  display: flex;
  margin-top: 0.5rem;
}

.button {
  margin-left: 10px;
}

.button:first-child {
  margin-left: 0;
}

.current-modus {
  text-align: center;
  margin-top: 0.2rem;
}

.current-modus span {
  margin-left: 0.1rem;
}

span {
  text-align: center;
  margin-top: 0.1rem;
  font-weight: bold;
}
</style>
