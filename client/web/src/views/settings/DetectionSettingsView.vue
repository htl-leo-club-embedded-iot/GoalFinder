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
import VibrationSensorManager from "@/components/settings/VibratiosensorManager.vue";
import LaserSensorManager from "@/components/settings/LaserSensorManager.vue";
import ToggleButton from "@/components/ToggleButton.vue";
import {useSettingsStore} from "@/stores/settings";
import InputForm from "@/components/InputForm.vue";

const settings = useSettingsStore();
</script>

<template>
  <div class="container">
    <h3>{{ $t("settings.sensors") }}</h3>
    <VibrationSensorManager/>
    <LaserSensorManager/>

    <h3>{{ $t("settings.hit_detection") }}</h3>
    <div class="label-container">
      <label>{{ $t("settings.distance_only_hit_detection") }}</label>
      <ToggleButton v-model="settings.distanceOnlyHitDetection">
        {{ settings.distanceOnlyHitDetection ? $t("word.on") : $t("word.off") }}
      </ToggleButton>
      <p class="description-text">{{ $t("settings.distance_only_hit_detection_desc") }}</p>
    </div>

    <div class="label-container">
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
</style>
