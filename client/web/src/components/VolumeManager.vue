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
  <div class="container">
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
  </div>
</template>

<style scoped>
.container {
  display: flex;
  flex-direction: column;
  align-items: center;
  width: 100%;
  padding: 1rem;
  box-sizing: border-box;
}

.volume-slider-control {
  display: flex;
  flex-direction: column;
  align-items: center;
  width: 100%;
  gap: 0.5rem;
}

.volume-slider-control label {
  text-align: center;
  font-weight: bold;
  display: block;
}

.button-container {
  display: flex;
  flex-wrap: wrap;
  gap: 0.5rem;
  justify-content: center;
  width: 100%;
}

.button-container1 {
  display: flex;
  flex-wrap: wrap;
  gap: 0.5rem;
  justify-content: center;
  width: 100%;
}

.button {
  margin-left: 0;
}

.current-value {
  text-align: center;
  margin-top: 0.5rem;
  font-weight: bold;
  width: 100%;
}

.sound-select {
  margin-top: 1.5rem;
  display: flex;
  flex-direction: column;
  align-items: center;
  width: 100%;
  gap: 0.5rem;
}

.sound-select label {
  text-align: center;
  font-weight: bold;
}
</style>
