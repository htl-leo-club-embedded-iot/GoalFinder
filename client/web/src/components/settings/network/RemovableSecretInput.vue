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
import { computed, onMounted, ref, watch } from "vue";
import Button from "@/components/Button.vue";
import EyeOpenIcon from "@/components/icons/EyeOpenIcon.vue";
import EyeClosedIcon from "@/components/icons/EyeClosedIcon.vue";
import KeyEnterIcon from "@/components/icons/KeyEnterIcon.vue";

/**
 * RemovableSecretInput encapsulates the removable password workflow
 * used by network and security-related settings.
 */
const props = withDefaults(defineProps<{
  id: string;
  modelValue: string;
  label: string;
  placeholder: string;
  storedValuePresent: boolean;
  removeLabel: string;
  showLabel: string;
  hideLabel: string;
  submitAriaLabel: string;
  minLength?: number;
  maxLength?: number;
  inputName?: string;
  autocomplete?: string;
  showSubmitButton?: boolean;
}>(), {
  minLength: 8,
  maxLength: 63,
  inputName: "password",
  autocomplete: "new-password",
  showSubmitButton: true,
});

const emit = defineEmits<{
  "update:modelValue": [value: string];
  change: [];
  commit: [];
  clear: [];
}>();

const showSecretInput = ref(false);
const showSecretValue = ref(false);
const hasLocalCommittedValue = ref(false);

const canCommit = computed(() => {
  return props.modelValue.length >= props.minLength && props.modelValue.length <= props.maxLength;
});

const passwordPresent = computed(() => {
  return (props.storedValuePresent || hasLocalCommittedValue.value) && !showSecretInput.value;
});

function onSecretInput(event: Event): void {
  const target = event.target as HTMLInputElement;
  emit("update:modelValue", target.value);
  emit("change");
}

function onToggleVisibility(): void {
  showSecretValue.value = !showSecretValue.value;
}

function onCommitSecret(): void {
  if (canCommit.value) {
    hasLocalCommittedValue.value = true;
    showSecretInput.value = false;
    emit("commit");
    emit("change");
  }
}

function onResetSecret(): void {
  showSecretInput.value = true;
  showSecretValue.value = false;
  hasLocalCommittedValue.value = false;
  emit("update:modelValue", "");
  emit("clear");
  emit("change");
}

onMounted(() => {
  showSecretInput.value = !props.storedValuePresent;
});

watch(() => props.storedValuePresent, (present) => {
  if (!hasLocalCommittedValue.value) {
    showSecretInput.value = !present;
  }
});

watch(() => props.modelValue, (value) => {
  const hasValidLength = value.length >= props.minLength && value.length <= props.maxLength;
  if (!hasValidLength) {
    hasLocalCommittedValue.value = false;
  }
});
</script>

<template>
  <div class="password-field">
    <label :for="id">{{ label }}</label>
    <div class="password-row" v-show="!passwordPresent">
      <input
        :id="id"
        :value="modelValue"
        :type="showSecretValue ? 'text' : 'password'"
        :placeholder="placeholder"
        :minlength="minLength"
        :maxlength="maxLength"
        :name="inputName"
        :autocomplete="autocomplete"
        pattern="^.{8,63}$|^$"
        @input="onSecretInput"
        @keyup.enter="onCommitSecret"
      />
      <Button
        class="control-btn password-toggle-btn"
        type="button"
        :aria-label="showSecretValue ? hideLabel : showLabel"
        @click="onToggleVisibility"
      >
        <EyeClosedIcon v-if="showSecretValue" class="password-toggle-icon" />
        <EyeOpenIcon v-else class="password-toggle-icon" />
      </Button>

      <Button
        v-if="showSubmitButton"
        class="control-btn password-submit-btn"
        :class="{ 'password-submit-btn-ready': canCommit }"
        :disabled="!canCommit"
        :aria-label="submitAriaLabel"
        type="button"
        @click="onCommitSecret"
      >
        <KeyEnterIcon class="password-submit-icon" />
      </Button>
    </div>

    <Button class="control-btn" type="button" v-show="passwordPresent" @click="onResetSecret">
      {{ removeLabel }}
    </Button>
  </div>
</template>

<style scoped>
.password-field {
  display: flex;
  flex-direction: column;
  gap: 0.3rem;
  width: 100%;
  max-width: 500px;
}

.password-field > label {
  text-align: center;
}

.password-row {
  display: flex;
  align-items: stretch;
  gap: 0.5rem;
  width: 100%;
}

.password-row > input {
  flex: 1;
  min-width: 0;
  padding: 0.5rem;
  border-radius: var(--corner-radius);
  border: 2px solid var(--border-color);
  background: var(--card-background-color);
  font-size: inherit;
  transition: 0.4s;
  color: var(--text-color);
  outline: none;
  text-align: center;
}

.password-row > input:focus {
  border-color: var(--accent-color);
}

.control-btn {
  min-width: 6rem;
  margin: auto;
}

.password-toggle-btn,
.password-submit-btn {
  background: var(--card-background-color);
  min-width: 2.75rem;
  width: 2.75rem;
  padding: 0.5rem;
  border-width: 2px;
  display: inline-flex;
  align-items: center;
  justify-content: center;
}

.password-toggle-icon,
.password-submit-icon {
  width: 1rem;
  height: 1rem;
}

.password-submit-btn.password-submit-btn-ready {
  background: var(--accent-color);
  border-color: var(--accent-color);
  color: var(--accent-contrast-color);
}

.password-submit-btn.password-submit-btn-ready:hover {
  background: var(--accent-color);
  border-color: var(--accent-color);
  color: var(--accent-contrast-color);
}
</style>
