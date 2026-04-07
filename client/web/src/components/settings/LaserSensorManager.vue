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
import InputForm from "@/components/InputForm.vue";
import { useClampedValue } from "@/models/clampedValue";

const settings = useSettingsStore();

// 30, 39, 45

function getSizeKey(mode: number): string {
  switch (mode) {
    case 1: return 'word.small';
    case 2: return 'word.middle';
    case 3: return 'word.normal';
    default: return 'word.normal';
  }
}

const getSimplifiedLevel = (actualSensitivity: number): number => actualSensitivity <= 150 ? 1 : (actualSensitivity <= 195 ? 2 : 3);

function getSimpleSize(simplifiedSensitivity: number) {
  switch(simplifiedSensitivity) {
    case 1: settings.vibrationSensorSensitivity = 150; break;
    case 2: settings.vibrationSensorSensitivity = 195; break;
    case 3: settings.vibrationSensorSensitivity = 225; break;
    default: settings.vibrationSensorSensitivity = 225; break;
  }
}

const distance = useClampedValue(
  () => settings.ballHitDetectionDistance,
  (v) => settings.ballHitDetectionDistance = v,
  150, 600
);
</script>

<template>
  <div id="led">
    <div class="label-container">
      <label>{{ $t("word.measuring")}} {{ $t("word.distance") }}</label>

      <div class="button-container" v-show="!settings.advancedSettingsEnabled">
        <button
          v-for="sensitivity in ([1, 2, 3, 4] as const)"
          :key="sensitivity"
          type="button"
          class="btn"
          :class="{ active: getSimplifiedLevel(settings.vibrationSensorSensitivity) === sensitivity }"
          @click="getSimpleSize(sensitivity)">
          {{ $t(getSizeKey(sensitivity)) }}
        </button>
      </div>
      
      <div class="button-container" v-show="settings.advancedSettingsEnabled">
        <InputForm type="number" class="button" v-model="distance" inputmode="numeric" min="150" max="600" step="10"></InputForm>
      </div>
    </div>
  </div>
</template>

<style scoped>
#led {
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
