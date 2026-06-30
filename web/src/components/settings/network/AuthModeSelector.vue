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
/**
 * AuthModeSelector provides a consistent selector for external network auth mode.
 */
const props = defineProps<{
  modelValue: string;
  label: string;
  options: Array<{
    value: string;
    label: string;
  }>;
}>();

const emit = defineEmits<{
  "update:modelValue": [value: string];
  change: [value: string];
}>();

function onSelectMode(mode: string): void {
  emit("update:modelValue", mode);
  emit("change", mode);
}
</script>

<template>
  <div class="selector-wrapper">
    <label>{{ label }}</label>
    <div class="selector-buttons">
      <button
        v-for="option in options"
        :key="option.value"
        type="button"
        class="selector-button"
        :class="{ active: modelValue === option.value }"
        @click="onSelectMode(option.value)"
      >
        {{ $t(option.label) }}
      </button>
    </div>
  </div>
</template>

<style scoped>
.selector-wrapper {
  width: 100%;
  max-width: 500px;
  display: flex;
  flex-direction: column;
  gap: 0.35rem;
}

.selector-wrapper > label {
  text-align: center;
}

.selector-buttons {
  display: flex;
  flex-wrap: wrap;
  justify-content: center;
  gap: 0.5rem;
}

.selector-button {
  padding: 0.45rem 1.1rem;
  border-radius: var(--corner-radius);
  border: 2px solid var(--border-color);
  background: var(--card-background-color);
  color: var(--text-color);
  cursor: pointer;
  font-size: 0.9rem;
  transition: border-color 0.3s, background-color 0.3s;
}

.selector-button:hover {
  border-color: var(--accent-color);
}

.selector-button.active {
  border-color: var(--accent-color);
  background: var(--accent-color);
  color: var(--accent-contrast-color);
}
</style>
