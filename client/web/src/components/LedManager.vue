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

function setLedMode(value: string, mode: number) {
  settings.ledModeStr = value;
  settings.ledMode = mode;
}
</script>

<template>
  <div class="container">
    <div id="led">
      <div class="label-container">
        <h3 for="modus">{{ $t("word.led_mode") }}</h3>
        <div class="button-container">
          <Button class="button" @click="setLedMode('Ein', 1)">{{ $t("word.on") }}</Button>
          <Button class="button" @click="setLedMode('Fade', 2)">{{ $t("word.fade") }}</Button>
          <Button class="button" @click="setLedMode('Blitzartig', 3)">{{ $t("word.flash") }}</Button>
          <Button class="button" @click="setLedMode('Turbo', 4)">{{ $t("word.turbo") }}</Button>
          <Button class="button" @click="setLedMode('Aus', 5)">{{ $t("word.off") }}</Button>
        </div>
      </div>
    </div>
    <div class="current-mode">
      <span>{{ $t("word.curr_mode") }}: {{ settings.ledModeStr }}</span>
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

#led {
  width: 100%;
}

.label-container {
  display: flex;
  flex-direction: column;
  align-items: center;
  margin-bottom: 1rem;
  margin-top: 0.5rem;
  width: 100%;
  gap: 0.5rem;
}

.label-container label {
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
  margin-top: 0.5rem;
}

.button {
  margin-left: 0;
}

.current-mode {
  text-align: center;
  width: 100%;
}

.current-mode span {
  font-weight: bold;
}

span {
  text-align: center;
  font-weight: bold;
}

h3 {
  margin: 0;
  margin-bottom: 1rem;
  color: var(--accent-color);
  font-size: 1.2rem;
}
</style>
