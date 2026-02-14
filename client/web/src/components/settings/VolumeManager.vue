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
import { useClampedValue } from "@/models/clampedValue";

const settings = useSettingsStore();

const volume = useClampedValue(
  () => settings.volume,
  (v) => settings.volume = v,
  0, 100
);

const setMetronomeSound = (value: number) => {
  settings.metronomeSound = value;
}

const setHitSound = (value: number) => {
  settings.hitSound = value;
};

const setMissSound = (value: number) => {
  settings.missSound = value;
};

</script>

<template>
  <div class="volume-slider-control">
    <h3>{{ $t("word.volume") }}</h3>
    <div class="button-container">
      <InputForm type="number" class="button" v-model="volume" inputmode="numeric" min="0" max="100" step="5"></InputForm>
    </div>
  </div>
  
  <div class="sound-select">
    <h3>{{ $t("settings.metronome_sound") }}</h3>
    <div class="button-container">
      <Button class="button" @click="setMetronomeSound(0)">{{ $t("word.sound") }} 1</Button>
      <Button class="button" @click="setMetronomeSound(1)">{{ $t("word.sound") }} 2</Button>
      <Button class="button" @click="setMetronomeSound(2)">{{ $t("word.sound") }} 3</Button>
    </div>
  </div>
  
  <div class="current-value">{{ $t("settings.current_metronome_sound") }}: {{ settings.metronomeSound + 1 }}</div>
  
  <div class="sound-select">
    <h3>{{ $t("settings.miss_sound") }}</h3>
    <div class="button-container">
      <Button class="button" @click="setMissSound(0)">{{ $t("word.sound") }} 1</Button>
      <Button class="button" @click="setMissSound(1)">{{ $t("word.sound") }} 2</Button>
      <Button class="button" @click="setMissSound(2)">{{ $t("word.sound") }} 3</Button>
    </div>
  </div>
  
  <div class="current-value">{{ $t("settings.current_miss_sound") }}: {{ settings.missSound + 1 }}</div>
</template>