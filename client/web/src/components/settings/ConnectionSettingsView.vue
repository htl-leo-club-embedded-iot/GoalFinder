<script setup lang="ts">
import InputForm from "@/components/InputForm.vue";
import Modal from "@/components/Modal.vue";
import Button from "@/components/Button.vue";
import EyeIcon from "@/components/icons/EyeIcon.vue";
import EyeOffIcon from "@/components/icons/EyeOffIcon.vue";
import KeyEnterIcon from "@/components/icons/KeyEnterIcon.vue";
import { useSettingsStore } from "@/stores/settings";
import { computed, ref, onMounted, useTemplateRef, watch } from "vue";

const settings = useSettingsStore();

const PASSWORD_MIN_LENGTH = 8;
const PASSWORD_MAX_LENGTH = 63;

const connectionModal = useTemplateRef<any>("connectionModal");
const dontShowAgain = ref(false);
const originalDeviceName = ref("");
const originalWifiPassword = ref("");
const originalDevicePassword = ref("");
const hasUserEditedConnectionSettings = ref(false);
const showWifiPasswordInput = ref(false);
const showWifiPassword = ref(false);
const wifiPasswordDraft = ref("");
const wifiPasswordPresent = computed(() => originalWifiPassword.value !== "" && !showWifiPasswordInput.value);
const canSubmitWifiPassword = computed(() => {
  return wifiPasswordDraft.value.length >= PASSWORD_MIN_LENGTH && wifiPasswordDraft.value.length <= PASSWORD_MAX_LENGTH;
});
const showDevicePasswordInput = ref(false);
const showDevicePassword = ref(false);
const devicePasswordDraft = ref("");
const devicePasswordPresent = computed(() => originalDevicePassword.value !== "" && !showDevicePasswordInput.value);
const canSubmitDevicePassword = computed(() => {
  return devicePasswordDraft.value.length >= PASSWORD_MIN_LENGTH && devicePasswordDraft.value.length <= PASSWORD_MAX_LENGTH;
});

onMounted(() => {
  originalDeviceName.value = settings.deviceName || "";
  originalWifiPassword.value = settings.wifiPassword || "";
  originalDevicePassword.value = settings.devicePassword || "";
  wifiPasswordDraft.value = settings.wifiPassword || "";
  devicePasswordDraft.value = settings.devicePassword || "";
  const stored = localStorage.getItem('connectionWarningDontShow');
  dontShowAgain.value = stored === 'true';
});

watch(
  () => [settings.deviceName || "", settings.wifiPassword || "", settings.devicePassword || ""],
  ([deviceName, wifiPassword, devicePassword]) => {
    if (!hasUserEditedConnectionSettings.value) {
      originalDeviceName.value = deviceName;
      originalWifiPassword.value = wifiPassword;
      originalDevicePassword.value = devicePassword;
      wifiPasswordDraft.value = wifiPassword;
      devicePasswordDraft.value = devicePassword;
    }
  },
  { immediate: true }
);

function markConnectionSettingsEdited() {
  hasUserEditedConnectionSettings.value = true;
}

function onResetWifiPasswordClick() {
  showWifiPasswordInput.value = true;
  showWifiPassword.value = false;
  wifiPasswordDraft.value = "";
  settings.wifiPassword = "";
  markConnectionSettingsEdited();
}

function onResetDevicePasswordClick() {
  showDevicePasswordInput.value = true;
  showDevicePassword.value = false;
  devicePasswordDraft.value = "";
  settings.devicePassword = "";
  markConnectionSettingsEdited();
}

function toggleWifiPasswordVisibility() {
  showWifiPassword.value = !showWifiPassword.value;
}

function toggleDevicePasswordVisibility() {
  showDevicePassword.value = !showDevicePassword.value;
}

function submitWifiPassword() {
  if (!canSubmitWifiPassword.value) {
    return;
  }

  settings.wifiPassword = wifiPasswordDraft.value;
  markConnectionSettingsEdited();
  onPrimaryEnter();
}

function submitDevicePassword() {
  if (!canSubmitDevicePassword.value) {
    return;
  }

  settings.devicePassword = devicePasswordDraft.value;
  markConnectionSettingsEdited();
  onPrimaryEnter();
}

function openConnectionModal() {
  connectionModal.value?.openDialog();
}

function closeConnectionModal(restart: boolean) {
  if (dontShowAgain.value) {
    localStorage.setItem('connectionWarningDontShow', 'true');
  }

  originalDeviceName.value = settings.deviceName || "";
  originalWifiPassword.value = settings.wifiPassword || "";
  originalDevicePassword.value = settings.devicePassword || "";
  wifiPasswordDraft.value = settings.wifiPassword || "";
  devicePasswordDraft.value = settings.devicePassword || "";
  showWifiPasswordInput.value = false;
  showWifiPassword.value = false;
  showDevicePasswordInput.value = false;
  showDevicePassword.value = false;
  hasUserEditedConnectionSettings.value = false;
  connectionModal.value?.closeDialog();

  if (restart) {
    settings.restartDevice();
  }
}

function onPrimaryEnter() {
  const currentDeviceName = settings.deviceName || "";
  const currentWifiPassword = settings.wifiPassword || "";
  const currentDevicePassword = settings.devicePassword || "";
  const hasConnectionChanges =
    currentDeviceName !== originalDeviceName.value ||
    currentWifiPassword !== originalWifiPassword.value ||
    currentDevicePassword !== originalDevicePassword.value;
  if (dontShowAgain.value) {
    settings.restartDevice();
  } else if (hasConnectionChanges) {
    openConnectionModal();
  } else {
    hasUserEditedConnectionSettings.value = false;
  }
}
</script>

<template>
  <form id="general-input" autocomplete="off">
    <InputForm v-model="settings.deviceName" :label="$t('word.device_name')"
           :placeholder="$t('description.device_name_description')" type="text"
        name="deviceName" autocomplete="off" @update:modelValue="markConnectionSettingsEdited" @enter="onPrimaryEnter" @blur="onPrimaryEnter"/>
               
    <div class="password-field">
      <label for="wifiPasswordInput">{{ $t('word.ssid_password') }}</label>
      <div class="password-row" v-show="!wifiPasswordPresent">
        <input
          id="wifiPasswordInput"
          v-model="wifiPasswordDraft"
          :type="showWifiPassword ? 'text' : 'password'"
          :placeholder="$t('description.ssid_password_description')"
          :minlength="PASSWORD_MIN_LENGTH"
          :maxlength="PASSWORD_MAX_LENGTH"
          pattern="^.{8,63}$|^$"
          name="wifiPassword"
          autocomplete="new-password"
          @input="markConnectionSettingsEdited"
          @keyup.enter="submitWifiPassword"
        />
        <Button class="control-btn password-toggle-btn"
          type="button" :aria-label="showWifiPassword ? $t('word.hide') : $t('word.show')"
          @click="toggleWifiPasswordVisibility">
            <EyeOffIcon v-if="showWifiPassword" key="hide" class="password-toggle-icon" />
            <EyeIcon v-else key="show" class="password-toggle-icon" />
        </Button>
        
        <Button
        class="control-btn password-submit-btn"
        :class="{ 'password-submit-btn-ready': canSubmitWifiPassword }"
        :disabled="!canSubmitWifiPassword"
        aria-label="Enter password"
        type="button"
        @click="submitWifiPassword"
        >
        <KeyEnterIcon class="password-submit-icon" />
      </Button>
    </div>
      
      <Button class="control-btn" id="resetWifiPassword" type="button"
      v-show="wifiPasswordPresent" @click="onResetWifiPasswordClick">{{ $t("settings.remove_password") }}</Button>
    </div>

    <div class="password-field">
      <label for="devicePasswordInput">{{ $t('word.device_password') }}</label>
      <div class="password-row" v-show="!devicePasswordPresent">
        <input
          id="devicePasswordInput"
          v-model="devicePasswordDraft"
          :type="showDevicePassword ? 'text' : 'password'"
          :placeholder="$t('description.device_password_description')"
          :minlength="PASSWORD_MIN_LENGTH"
          :maxlength="PASSWORD_MAX_LENGTH"
          pattern="^.{8,63}$|^$"
          name="devicePassword"
          autocomplete="new-password"
          @input="markConnectionSettingsEdited"
          @keyup.enter="submitDevicePassword"
        />
        <Button class="control-btn password-toggle-btn"
          type="button" :aria-label="showDevicePassword ? $t('word.hide') : $t('word.show')"
          @click="toggleDevicePasswordVisibility">
            <EyeOffIcon v-if="showDevicePassword" key="device-hide" class="password-toggle-icon" />
            <EyeIcon v-else key="device-show" class="password-toggle-icon" />
        </Button>

        <Button
        class="control-btn password-submit-btn"
        :class="{ 'password-submit-btn-ready': canSubmitDevicePassword }"
        :disabled="!canSubmitDevicePassword"
        aria-label="Enter password"
        type="button"
        @click="submitDevicePassword"
        >
        <KeyEnterIcon class="password-submit-icon" />
      </Button>
    </div>

      <Button class="control-btn" id="resetDevicePassword" type="button"
      v-show="devicePasswordPresent" @click="onResetDevicePasswordClick">{{ $t("settings.remove_password") }}</Button>
    </div>
    
  </form>

  <Modal ref="connectionModal" :title="$t('settings.restart_title')" centered hide-close-button>
    <p>{{ $t('settings.restart_desc') }}</p>
    <label class="dont-show-again">
      <input type="checkbox" v-model="dontShowAgain" />
      {{ $t('settings.restart_always') }}
    </label>
    <div class="connection-modal-actions">
      <Button primary @click="closeConnectionModal(true)">{{ $t('settings.restart_now') }}</Button>
      <Button primary class="invert-hover" @click="closeConnectionModal(false)">{{ $t('settings.restart_later') }}</Button>
    </div>
  </Modal>
</template>

<style scoped>
#general-input {
  display: flex;
  flex-direction: column;
  gap: 1rem;
  width: 100%;
  max-width: 500px;
  margin-top: 0;
}

.dont-show-again {
  display: flex;
  align-items: center;
  gap: 0.5rem;
  margin-top: 1rem;
}

.connection-modal-actions {
  margin-top: 1rem;
  display: flex;
  justify-content: center;
  gap: 1rem;
}

.connection-modal-actions ::v-deep button.invert-hover {
  background: transparent;
  color: var(--accent-color);
  border: 2px solid var(--accent-color);
}
.connection-modal-actions ::v-deep button.invert-hover:hover {
  background: var(--accent-color);
  color: white;
  border: 2px solid transparent;
}

.control-btn {
  min-width: 6rem;
  margin: auto;
}

.password-field {
  display: flex;
  flex-direction: column;
  gap: 0.3rem;
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

.password-toggle-btn {
  background: var(--card-background-color);
  min-width: 2.75rem;
  width: 2.75rem;
  padding: 0.5rem;
  border-width: 2px;
  display: inline-flex;
  align-items: center;
  justify-content: center;
}

.password-toggle-icon {
  width: 1rem;
  height: 1rem;
}

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

.password-submit-icon {
  width: 1rem;
  height: 1rem;
}

.password-submit-btn.password-submit-btn-ready {
  background: var(--accent-color);
  border-color: var(--accent-color);
  color: white;
}

.password-submit-btn.password-submit-btn-ready:hover {
  background: var(--accent-color);
  border-color: var(--accent-color);
  color: white;
}
</style>
