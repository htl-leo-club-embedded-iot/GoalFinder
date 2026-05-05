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
defineProps<{
  modelValue: boolean;
  label: string;
}>();

defineEmits<{
  'update:modelValue': [value: boolean];
  change: [];
}>();
</script>

<template>
  <label class="checkbox-label">
    <input
      type="checkbox"
      :checked="modelValue"
      @change="(e) => {
        $emit('update:modelValue', (e.target as HTMLInputElement).checked);
        $emit('change');
      }"
    />
    {{ $t(label) }}
  </label>
</template>

<style scoped>
.checkbox-label {
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 0.5rem;
  cursor: pointer;
  user-select: none;
}

input[type="checkbox"] {
  cursor: pointer;
  accent-color: var(--accent-color);
}

input[type="checkbox"]:hover {
  filter: brightness(1.2);
}
</style>
