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
import InputForm from "@/components/InputForm.vue";
import {computed} from "vue";
import {useClampedValue} from "@/models/clampedValue";

const settings = useSettingsStore();

function setLedMode(mode: number) {
  settings.ledMode = mode;
}

function getLedModeKey(mode: number): string {
  switch (mode) {
    case 1: return 'word.on';
    case 2: return 'word.fade';
    case 3: return 'word.flash';
    case 4: return 'word.turbo';
    case 5: return 'word.off';
    default: return 'word.off';
  }
}

function getLedBrightnessKey(mode: number): string {
  switch (mode) {
    case 1: return 'word.low';
    case 2: return 'word.medium';
    case 3: return 'word.high';
    case 4: return 'word.max';
    default: return 'word.medium';
  }
}

function getSimplifiedBrightnessLevel(actualBrightness: number): number {
  if (actualBrightness <= 37.5) return 1;
  if (actualBrightness <= 62.5) return 2;
  if (actualBrightness <= 87.5) return 3;
  return 4;
}

function setLedBrightness(simplifiedLevel: number) {
  switch(simplifiedLevel) {
    case 1: settings.ledBrightness = 25; break;
    case 2: settings.ledBrightness = 50; break;
    case 3: settings.ledBrightness = 75; break;
    case 4: settings.ledBrightness = 100; break;
    default: settings.ledBrightness = 75; break;
  }
}

const ledBrightness = useClampedValue(
  () => settings.ledBrightness,
  (v) => settings.ledBrightness = v,
  0, 100
);
</script>

<template>
  <div class="container">
    <div id="led">
      <h3>{{ $t("word.led_mode") }}</h3>
      <div class="button-container">
        <button
          v-for="ledMode in ([1, 2, 3, 4, 5] as const)"
          :key="ledMode"
          type="button"
          class="btn"
          :class="{ active: settings.ledMode === ledMode }"
          @click="setLedMode(ledMode)">
          {{ $t(getLedModeKey(ledMode)) }}
        </button>
      </div>
    </div>

    <div id="brightness" v-show="!settings.advancedSettingsEnabled">
      <div class="label-container">
        <h3>{{ $t("word.brightness") }}</h3>
        <div class="button-container">
          <div class="button-container">
            <button
              v-for="ledBrightness in ([1, 2, 3, 4] as const)"
              :key="ledBrightness"
              type="button"
              class="btn"
              :class="{ active: getSimplifiedBrightnessLevel(settings.ledBrightness) === ledBrightness }"
              @click="setLedBrightness(ledBrightness)">
              {{ $t(getLedBrightnessKey(ledBrightness)) }}
            </button>
          </div>
        </div>
      </div>
    </div>

    <div id="brightness" v-show="settings.advancedSettingsEnabled">
      <div class="label-container">
        <h3>{{ $t("word.brightness") }}</h3>
        <div class="button-container">
          <InputForm type="number" class="button" v-model="ledBrightness" inputmode="numeric" min="0" max="100" step="5"></InputForm>
        </div>
      </div>
    </div>
  </div>
</template>

<style scoped>
#led {
  width: 100%;
  text-align: center;
  display: flex;
  flex-direction: column;
  gap: 0.7rem;
}

#brightness {
  width: 100%;
}

.btn {
  padding: 0.45rem 1.1rem;
  border-radius: var(--corner-radius);
  border: 2px solid var(--border-color);
  background: var(--card-background-color);
  color: var(--text-color);
  cursor: pointer;
  font-size: 0.9rem;
  transition: border-color 0.3s, background-color 0.3s;
}

.btn:hover {
  border-color: var(--accent-color);
}

.btn.active {
  border-color: var(--accent-color);
  background-color: var(--accent-color);
  color: #fff;
}
</style>
