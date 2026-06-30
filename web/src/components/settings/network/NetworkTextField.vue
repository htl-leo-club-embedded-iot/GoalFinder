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
import { computed } from "vue";
import InputForm from "@/components/InputForm.vue";

/**
 * NetworkTextField renders a labeled text-like input with optional helper text.
 */
const props = withDefaults(defineProps<{
  modelValue: string;
  label: string;
  placeholder?: string;
  description?: string;
  type?: string;
  name?: string;
  autocomplete?: string;
  inputmode?: string;
  minlength?: number;
  maxlength?: number;
  pattern?: string;
}>(), {
  type: "text",
  name: "",
  autocomplete: "off",
  inputmode: "text",
  placeholder: "",
  description: "",
  minlength: undefined,
  maxlength: undefined,
  pattern: "",
});

const emit = defineEmits<{
  "update:modelValue": [value: string];
  change: [];
  blur: [];
  enter: [];
}>();

const fieldModel = computed({
  get: () => props.modelValue,
  set: (value: string) => {
    emit("update:modelValue", value);
    emit("change");
  },
});
</script>

<template>
  <div class="network-field">
    <InputForm
      v-model="fieldModel"
      :label="label"
      :placeholder="placeholder"
      :type="type"
      :name="name"
      :autocomplete="autocomplete"
      :inputmode="inputmode"
      :minlength="minlength"
      :maxlength="maxlength"
      :pattern="pattern"
      @blur="emit('blur')"
      @enter="emit('enter')"
    />
    <p v-if="description" class="field-description">{{ description }}</p>
  </div>
</template>

<style scoped>
.network-field {
  width: 100%;
  max-width: 500px;
}

.field-description {
  text-align: center;
  color: var(--text-color-secondary);
  margin: 0.25rem 0 0 0;
  font-size: 0.85rem;
}
</style>
