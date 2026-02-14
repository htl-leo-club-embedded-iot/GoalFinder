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
import { useI18n } from 'vue-i18n';

const { locale } = useI18n();

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

// Theme: 'auto' | 'light' | 'dark'
const selectedTheme = ref<'auto' | 'light' | 'dark'>('auto');

// Language
const selectedLanguage = ref('de');

const languages = [
  { code: 'de', label: 'Deutsch' },
  { code: 'en', label: 'English' },
];

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

function applyTheme(theme: 'auto' | 'light' | 'dark') {
  selectedTheme.value = theme;
  localStorage.setItem('theme', theme);
  if (theme === 'auto') {
    document.documentElement.removeAttribute('data-theme');
  } else {
    document.documentElement.setAttribute('data-theme', theme);
  }
}

function applyLanguage(lang: string) {
  selectedLanguage.value = lang;
  locale.value = lang;
  localStorage.setItem('language', lang);
}

onMounted(() => {
  const savedColor = localStorage.getItem('accent-color');
  if (savedColor) {
    selectedColor.value = savedColor;
    customColor.value = savedColor;
    document.documentElement.style.setProperty('--accent-color', savedColor);
  }

  const savedTheme = localStorage.getItem('theme');
  if (savedTheme === 'light' || savedTheme === 'dark') {
    selectedTheme.value = savedTheme;
  } else {
    selectedTheme.value = 'auto';
  }

  const savedLang = localStorage.getItem('language');
  if (savedLang) {
    selectedLanguage.value = savedLang;
  } else {
    selectedLanguage.value = locale.value;
  }
});
</script>

<template>
  <div class="web-app-settings">

    <!-- Theme -->
    <div class="setting-section">
      <h4>{{ $t("settings.theme") }}</h4>
      <div class="theme-options">
        <button
          v-for="theme in (['light', 'dark', 'auto'] as const)"
          :key="theme"
          class="theme-btn"
          :class="{ active: selectedTheme === theme }"
          @click="applyTheme(theme)"
        >
          {{ $t(`settings.theme_${theme}`) }}
        </button>
      </div>
    </div>

    <!-- Language -->
    <div class="setting-section">
      <h4>{{ $t("settings.language") }}</h4>
      <div class="language-options">
        <button
          v-for="lang in languages"
          :key="lang.code"
          class="lang-btn"
          :class="{ active: selectedLanguage === lang.code }"
          @click="applyLanguage(lang.code)"
        >
          {{ lang.label }}
        </button>
      </div>
    </div>

    <!-- Accent Color -->
    <div class="setting-section">
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

  </div>
</template>

<style scoped>
h4 {
    color: var(--text-color) !important;
}

.web-app-settings {
  display: flex;
  flex-direction: column;
  align-items: center;
  width: 100%;
  gap: 1.25rem;
}

.setting-section {
  display: flex;
  flex-direction: column;
  align-items: center;
  width: 100%;
}

.setting-section h4 {
  margin: 0 0 0.75rem 0;
}

/* Theme buttons */
.theme-options,
.language-options {
  display: flex;
  gap: 0.5rem;
  flex-wrap: wrap;
  justify-content: center;
}

.theme-btn,
.lang-btn {
  padding: 0.45rem 1.1rem;
  border-radius: var(--corner-radius-secondary);
  border: 2px solid var(--border-color);
  background: var(--card-background-color);
  color: var(--text-color);
  cursor: pointer;
  font-size: 0.9rem;
  transition: border-color 0.3s, background-color 0.3s;
}

.theme-btn:hover,
.lang-btn:hover {
  border-color: var(--accent-color);
}

.theme-btn.active,
.lang-btn.active {
  border-color: var(--accent-color);
  background-color: var(--accent-color);
  color: #fff;
}

/* Accent color swatches */
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