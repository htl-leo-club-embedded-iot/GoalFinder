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
import InputForm from "@/components/InputForm.vue";
import { useSettingsStore } from "@/stores/settings";
import { useClampedValue } from "@/models/clampedValue";

const settings = useSettingsStore();

function getModeKey(mode: number): string {
  switch (mode) {
    case 1: return 'word.low';
    case 2: return 'word.medium';
    case 3: return 'word.high';
    case 4: return 'word.max';
    default: return 'word.medium';
  }
}

const getSimplifiedSensitivityLevel = (actualSensitivity: number): number => actualSensitivity <= 25 ? 1 : (actualSensitivity <= 50 ? 2 : (actualSensitivity <= 75 ? 3 : 4));

function setSimpleSensitivity(simplifiedSensitivity: number) {
  switch(simplifiedSensitivity) {
    case 1: settings.vibrationSensorSensitivity = 25; break;
    case 2: settings.vibrationSensorSensitivity = 50; break;
    case 3: settings.vibrationSensorSensitivity = 75; break;
    case 4: settings.vibrationSensorSensitivity = 100; break;
    default: settings.vibrationSensorSensitivity = 75; break;
  }
}

const sensitivity = useClampedValue(
  () => settings.vibrationSensorSensitivity,
  (v) => settings.vibrationSensorSensitivity = v,
  0, 100
);
</script>

<template>
  <div>
    <div id="vb">
      <div class="label-container" v-show="!settings.advancedSettingsEnabled">
        <label>{{ $t("word.sensitivity") }}</label>
        <div class="button-container">
          <button
            v-for="sensitivity in ([1, 2, 3, 4] as const)"
            :key="sensitivity"
            type="button"
            class="btn"
            :class="{ active: getSimplifiedSensitivityLevel(settings.vibrationSensorSensitivity) === sensitivity }"
            @click="setSimpleSensitivity(sensitivity)">
            {{ $t(getModeKey(sensitivity)) }}
          </button>
        </div>
      </div>

      <div class="button-container" v-show="settings.advancedSettingsEnabled">
        <InputForm type="number" class="button" v-model="sensitivity" inputmode="numeric" min="0" max="100" step="1"></InputForm>
      </div>
    </div>
  </div>
</template>

<style scoped>
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
