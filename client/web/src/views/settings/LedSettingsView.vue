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

const ledBrightness = useClampedValue(
  () => settings.ledBrightness,
  (v) => settings.ledBrightness = v,
  0, 100
);

const ledModeKey = computed(() => {
  switch (settings.ledMode) {
    case 1: return 'word.on';
    case 2: return 'word.fade';
    case 3: return 'word.flash';
    case 4: return 'word.turbo';
    case 5: return 'word.off';
    default: return 'word.off';
  }
});
</script>

<template>
  <div class="container">
    <div id="led">
      <div class="label-container">
        <h3 for="modus">{{ $t("word.led_mode") }}</h3>
        <div class="button-container">
          <Button class="button" @click="setLedMode(1)">{{ $t("word.on") }}</Button>
          <Button class="button" @click="setLedMode(2)">{{ $t("word.fade") }}</Button>
          <Button class="button" @click="setLedMode(3)">{{ $t("word.flash") }}</Button>
          <Button class="button" @click="setLedMode(4)">{{ $t("word.turbo") }}</Button>
          <Button class="button" @click="setLedMode(5)">{{ $t("word.off") }}</Button>
        </div>
      </div>
    </div>
    <div class="current-mode">
      <p style="margin: 0;">{{ $t("word.curr_mode") }}: {{ $t(ledModeKey) }}</p>
    </div>
    <div id="brightness">
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
h3 {
  margin-top: 0.5rem;
}

#led {
  width: 100%;
}

#brightness {
  width: 100%;
}
</style>
