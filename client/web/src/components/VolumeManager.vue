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

const setVolume = (value: number) => {
  settings.volume = value;
};

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
    <label>{{ $t("word.volume") }}</label>
    <div class="button-container1">
      <Button class="button" @click="setVolume(0)">0%</Button>
      <Button class="button" @click="setVolume(25)">25%</Button>
      <Button class="button" @click="setVolume(50)">50%</Button>
      <Button class="button" @click="setVolume(75)">75%</Button>
      <Button class="button" @click="setVolume(100)">100%</Button>
    </div>
  </div>
  <div class="current-value">
    {{ $t("word.curr_volume") }}: {{ settings.volume }}%
  </div>

  <div class="sound-select">
    <label>Metronom Geräusch</label>
    <div class="button-container">
      <Button class="button" @click="setMetronomeSound(0)">Ton 1</Button>
      <Button class="button" @click="setMetronomeSound(1)">Ton 2</Button>
      <Button class="button" @click="setMetronomeSound(2)">Ton 3</Button>
    </div>
  </div>
  <div class="current-value">
    Derzeitiges Metronom Geräusch: {{ settings.metronomeSound + 1 }}
  </div>

  <div class="sound-select">
    <label>Fehlschuss Ton</label>
    <div class="button-container">
      <Button class="button" @click="setMissSound(0)">Ton 1</Button>
      <Button class="button" @click="setMissSound(1)">Ton 2</Button>
      <Button class="button" @click="setMissSound(2)">Ton 3</Button>
    </div>
  </div>
  <div class="current-value">
   Derzeitiger Fehlschuss Ton : {{ settings.missSound + 1 }}
  </div>
</template>

<style scoped>
.volume-slider-control {
  display: flex;
  align-items: center;
  flex-wrap: wrap;
  margin-bottom: 1rem; 
}

.button-container {
  margin-left: auto;
  display: flex;
  gap: 0.5rem;
}

.button-container1 {
  margin-top: 0.5rem;
  margin-left: auto;
  display: flex;
  gap: 0.5rem;
}

.button {
  margin-left: 0;
}

.current-value {
  text-align: center;
  margin-top: 0.5rem;
  font-weight: bold;
}

.sound-select {
  display: flex;
  align-items: center;
  margin-top: 1.5rem;
}

.sound-select label {
  margin-right: 1rem;
}
</style>
