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
import { useClampedValue } from "@/models/clampedValue";
import InputForm from "@/components/InputForm.vue";

const settings = useSettingsStore();

type DetectionPreset = {
  ballHitDetectionDistance: number;
  vibrationSensorSensitivity: number;
  afterHitTimeout: number;
};

const PRESETS: DetectionPreset[] = [
  { ballHitDetectionDistance: 220, vibrationSensorSensitivity: 60, afterHitTimeout: 2 },
  { ballHitDetectionDistance: 180, vibrationSensorSensitivity: 60, afterHitTimeout: 2 },
  { ballHitDetectionDistance: 220, vibrationSensorSensitivity: 80, afterHitTimeout: 4 },
  { ballHitDetectionDistance: 180, vibrationSensorSensitivity: 80, afterHitTimeout: 4 }
];

function getModeKey(mode: number): string {
  switch (mode) {
    case 1: return 'settings.basket_mounted';
    case 2: return 'settings.basket_mounted_small';
    case 3: return 'settings.movable_basket';
    case 4: return 'settings.movable_basket_small';
    default: return 'settings.basket_mounted';
  }
}

function getActivePresetIndex(): number {
  const current = {
    ballHitDetectionDistance: settings.ballHitDetectionDistance,
    vibrationSensorSensitivity: settings.vibrationSensorSensitivity,
    afterHitTimeout: settings.afterHitTimeout,
  };

  for (let i = 0; i < PRESETS.length; i++) {
    const preset = PRESETS[i];
    if (current.ballHitDetectionDistance === preset.ballHitDetectionDistance
      && current.vibrationSensorSensitivity === preset.vibrationSensorSensitivity
      && current.afterHitTimeout === preset.afterHitTimeout) {
      return i;
    }
  }

  return -1;
}

function applyPreset(mode: number) {
  let preset: DetectionPreset;

  switch (mode) {
    case 1: preset = PRESETS[0]; break;
    case 2: preset = PRESETS[1]; break;
    case 3: preset = PRESETS[2]; break;
    case 4: preset = PRESETS[3]; break;
    default: preset = PRESETS[1]; break;
  }

  settings.ballHitDetectionDistance = preset.ballHitDetectionDistance;
  settings.vibrationSensorSensitivity = preset.vibrationSensorSensitivity;
  settings.afterHitTimeout = preset.afterHitTimeout;
  settings.scheduleSave();
}

const sensitivity = useClampedValue(
  () => settings.vibrationSensorSensitivity,
  (v) => settings.vibrationSensorSensitivity = v,
  0, 100
);

const distance = useClampedValue(
  () => settings.ballHitDetectionDistance,
  (v) => settings.ballHitDetectionDistance = v,
  100, 350
);
</script>

<template>
  <div class="container">
    <h3 v-show="settings.advancedSettingsEnabled">{{ $t("settings.sensors") }}</h3>
    <div class="label-container" v-show="settings.advancedSettingsEnabled">
      <label>{{ $t("word.sensitivity") }}</label>
      <InputForm type="number" class="button" v-model="sensitivity" inputmode="numeric" min="0" max="100" step="1"></InputForm>
    </div>
    <div class="label-container" v-show="settings.advancedSettingsEnabled">
      <label>{{ $t("word.measuring")}} {{ $t("word.distance") }}</label>
      <InputForm type="number" class="button" v-model="distance" inputmode="numeric" min="150" max="600" step="10"></InputForm>
    </div>

    <h3>{{ $t("settings.hit_detection") }}</h3>
    <div class="label-container" v-show="settings.advancedSettingsEnabled">
      <label for="after-hit-timeout-input">{{ $t("settings.after_hit_timeout") }}</label>
      <InputForm
        id="after-hit-timeout-input"
        type="number"
        v-model.number="settings.afterHitTimeout"
        min="0"
        max="60"
        @change="settings.scheduleSave()"
      />
      <p class="description-text">{{ $t("settings.after_hit_timeout_desc") }}</p>
    </div>

    <div class="label-container" v-show="!settings.advancedSettingsEnabled">
      <label>{{ $t("settings.basket_mode") }}</label>
      <button
        v-for="mode in ([1, 2, 3, 4] as const)"
        :key="mode"
        type="button"
        class="btn"
        :class="{ active: getActivePresetIndex() === (mode - 1) }"
        @click="applyPreset(mode)">
        {{ $t(getModeKey(mode)) }}
      </button>
    </div>
  </div>
</template>

<style scoped>
.description-text {
  text-align: center;
  font-size: 0.85rem;
  opacity: 0.7;
  margin: 0.25rem 0 0 0;
  max-width: 500px;
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
