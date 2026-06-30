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
import { onMounted } from "vue";
import {useSettingsStore} from "@/stores/settings";
import InputForm from "@/components/InputForm.vue";
import { useClampedValue } from "@/models/clampedValue";

const settings = useSettingsStore();

const volume = useClampedValue(
  () => settings.volume,
  (v) => settings.volume = v,
  0, 100
);

function getVolumeKey(level: number): string {
  switch (level) {
    case 1: return 'word.off';
    case 2: return 'word.low';
    case 3: return 'word.medium';
    case 4: return 'word.max';
    default: return 'word.off';
  }
}

const getSimplifiedVolumeLevel = (actualVolume: number): number =>
  actualVolume === 0 ? 1 : (actualVolume <= 25 ? 2 : (actualVolume <= 50 ? 3 : 4));

function setVolumeSimplified(simplifiedLevel: number) {
  switch(simplifiedLevel) {
    case 1: settings.volume = 0; break;
    case 2: settings.volume = 25; break;
    case 3: settings.volume = 50; break;
    case 4: settings.volume = 100; break;
    default: settings.volume = 50; break;
  }
}

const setMetronomeSound = (value: number) => {
  settings.metronomeSound = value;
}

const setHitSound = (value: number) => {
  settings.hitSound = value;
};

const setMissSound = (value: number) => {
  settings.missSound = value;
};

const setWaitingSound = (value: number) => {
  settings.waitingSound = value;
};

onMounted(() => {
  settings.getSettings();
});

const time = useClampedValue(
  () => settings.metronomeSoundDelay,
  (v) => settings.metronomeSoundDelay = v,
  500, 3000
);
</script>

<template>
  <div class="volume-slider-control">
    <h3>{{ $t("word.volume") }}</h3>
    <div class="button-container" v-show="!settings.advancedSettingsEnabled">
      <button
        v-for="volumeLevel in ([1, 2, 3, 4] as const)"
        :key="volumeLevel"
        type="button"
        class="btn"
        :class="{ active: getSimplifiedVolumeLevel(settings.volume) === volumeLevel }"
        @click="setVolumeSimplified(volumeLevel)">
        {{ $t(getVolumeKey(volumeLevel)) }}
      </button>
    </div>
    <div class="button-container" v-show="settings.advancedSettingsEnabled">
      <InputForm type="number" class="button" v-model="volume" inputmode="numeric" min="0" max="100" step="5"></InputForm>
    </div>
  </div>
  
  <div class="sound-select">
    <h3>{{ $t("settings.metronome_sound") }}</h3>
    <div class="button-container">
      <button
        v-for="metronomeSound in ([0, 1, 2, 3] as const)"
        :key="metronomeSound"
        type="button"
        class="sound-btn"
        :class="{ active: settings.metronomeSound === metronomeSound }"
        @click="setMetronomeSound(metronomeSound)">
        {{ $t("word.sound") }} {{ metronomeSound + 1 }}
      </button>
    </div>
  </div>
    
  <div class="sound-select"> 
    <h3>{{ $t("settings.hit_sound") }}</h3>
    <div class="button-container">
      <button
        v-for="hitSound in ([0, 1, 2] as const)"
        :key="hitSound"
        type="button"
        class="sound-btn"
        :class="{ active: settings.hitSound === hitSound}"
        @click="setHitSound(hitSound)">
        {{  $t("word.sound") }} {{ hitSound + 1 }}
      </button>
    </div>
  </div>

  <div class="sound-select">
    <h3>{{ $t("settings.miss_sound") }}</h3>
    <div class="button-container">
      <button
        v-for="missSound in ([0, 1, 2] as const)"
        :key="missSound"
        type="button"
        class="sound-btn"
        :class="{ active: settings.missSound === missSound}"
        @click="setMissSound(missSound)">
        {{  $t("word.sound") }} {{ missSound + 1 }}
      </button>
    </div>
  </div>

  <div class="sound-select" v-show="settings.advancedSettingsEnabled">
    <h3>{{ $t("settings.waiting_sound") }}</h3>
    <div class="button-container">
      <button
        v-for="waitingSound in ([0, 1, 2] as const)"
        :key="waitingSound"
        type="button"
        class="sound-btn"
        :class="{ active: settings.waitingSound === waitingSound}"
        @click="setWaitingSound(waitingSound)">
        {{  $t("word.sound") }} {{ waitingSound + 1 }}
      </button>
    </div>
  </div>

  <div class="timing" v-show="settings.advancedSettingsEnabled">
    <h3>{{ $t("word.timing") }}</h3>
    <label>{{ $t("settings.between_sound")}} (ms)</label>
    <div class="button-container">
      <InputForm type="number" class="button" v-model="time" inputmode="numeric" min="150" max="600" step="10"></InputForm>
    </div>
  </div>
</template>

<style scoped>
.sound-btn {
  padding: 0.45rem 1.1rem;
  border-radius: var(--corner-radius);
  border: 2px solid var(--border-color);
  background: var(--card-background-color);
  color: var(--text-color);
  cursor: pointer;
  font-size: 0.9rem;
  transition: border-color 0.3s, background-color 0.3s;
}

.sound-btn:hover {
  border-color: var(--accent-color);
}

.sound-btn.active {
  border-color: var(--accent-color);
  background-color: var(--accent-color);
  color: #fff;
}

.timing {
  margin-top: 1rem;
  text-align: center;
  display: flex;
  flex-direction: column;
  gap: 0.5rem;
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