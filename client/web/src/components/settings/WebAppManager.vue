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
import { ref, onMounted } from 'vue';

const presetColors = [
  { name: 'Cornflower Blue', value: 'cornflowerblue' },
  { name: 'Purple', value: '#6B6ED3' },
  { name: 'Green', value: '#4CAF50' },
  { name: 'Red', value: '#E53935' },
  { name: 'Orange', value: '#FF9800' },
  { name: 'Teal', value: '#009688' },
];

const customColor = ref('#6495ED');
const selectedColor = ref('cornflowerblue');

function applyColor(color: string) {
  selectedColor.value = color;
  document.documentElement.style.setProperty('--accent-color', color);
  localStorage.setItem('accent-color', color);
}

function onCustomColorInput(event: Event) {
  const target = event.target as HTMLInputElement;
  customColor.value = target.value;
  applyColor(target.value);
}

onMounted(() => {
  const saved = localStorage.getItem('accent-color');
  if (saved) {
    selectedColor.value = saved;
    customColor.value = saved;
    document.documentElement.style.setProperty('--accent-color', saved);
  }
});
</script>

<template>
  <div class="accent-color-setting">
    <h4>{{ $t("settings.accent_color") }}</h4>
    <div class="color-options">
      <button
        v-for="color in presetColors"
        :key="color.value"
        class="color-swatch"
        :class="{ active: selectedColor === color.value }"
        :style="{ backgroundColor: color.value }"
        :title="color.name"
        @click="applyColor(color.value)"
      />
      <label class="color-swatch custom-color-swatch" :title="'Custom'">
        <input type="color" :value="customColor" @input="onCustomColorInput" />
      </label>
    </div>
  </div>
</template>

<style scoped>
h4 {
    color: var(--text-color) !important;
}

.accent-color-setting {
  display: flex;
  flex-direction: column;
  align-items: center;
  width: 100%;
}

.accent-color-setting h4 {
  margin: 0 0 0.75rem 0;
  color: var(--accent-color);
}

.color-options {
  display: flex;
  flex-wrap: wrap;
  gap: 0.6rem;
  justify-content: center;
}

.color-swatch {
  width: 2.5rem;
  height: 2.5rem;
  border-radius: 50%;
  border: 3px solid var(--border-color);
  cursor: pointer;
  transition: border-color 0.3s, transform 0.2s;
  padding: 0;
  background: transparent;
}

.color-swatch:hover {
  transform: scale(1.15);
}

.color-swatch.active {
  border-color: var(--text-color);
  transform: scale(1.15);
}

.custom-color-swatch {
  position: relative;
  overflow: hidden;
  background: conic-gradient(from 0deg, red, yellow, lime, aqua, blue, magenta, red);
  background-clip: padding-box;
}

.custom-color-swatch input[type="color"] {
  position: absolute;
  inset: 0;
  width: 100%;
  height: 100%;
  opacity: 0;
  cursor: pointer;
  border: none;
  padding: 0;
}
</style>