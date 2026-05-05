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
 * NetworkTextareaField renders multi-line network-related text input sections,
 * such as certificate and key content placeholders.
 */
const props = withDefaults(defineProps<{
  modelValue: string;
  label: string;
  placeholder?: string;
  description?: string;
  rows?: number;
}>(), {
  placeholder: "",
  description: "",
  rows: 4,
});

const emit = defineEmits<{
  "update:modelValue": [value: string];
  change: [];
}>();

function onTextareaInput(event: Event): void {
  const target = event.target as HTMLTextAreaElement;
  emit("update:modelValue", target.value);
  emit("change");
}
</script>

<template>
  <div class="textarea-field">
    <label>{{ label }}</label>
    <textarea
      :value="modelValue"
      :placeholder="placeholder"
      :rows="rows"
      @input="onTextareaInput"
    />
    <p v-if="description" class="field-description">{{ description }}</p>
  </div>
</template>

<style scoped>
.textarea-field {
  width: 100%;
  max-width: 500px;
  display: flex;
  flex-direction: column;
  gap: 0.3rem;
}

.textarea-field > label {
  text-align: center;
}

.textarea-field > textarea {
  resize: vertical;
  min-height: 5rem;
  padding: 0.5rem;
  border-radius: var(--corner-radius);
  border: 2px solid var(--border-color);
  background: var(--card-background-color);
  color: var(--text-color);
  font-size: inherit;
  outline: none;
}

.textarea-field > textarea:focus {
  border-color: var(--accent-color);
}

.field-description {
  text-align: center;
  color: var(--text-color-secondary);
  margin: 0;
  font-size: 0.85rem;
}
</style>
