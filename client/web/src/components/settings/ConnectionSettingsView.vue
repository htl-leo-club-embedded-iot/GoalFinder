<script setup lang="ts">
import InputForm from "@/components/InputForm.vue";
import Modal from "@/components/Modal.vue";
import Button from "@/components/Button.vue";
import EyeClosedIcon from "@/components/icons/EyeClosedIcon.vue";
import EyeOpenIcon from "@/components/icons/EyeOpenIcon.vue";
import KeyEnterIcon from "@/components/icons/KeyEnterIcon.vue";
import { useSettingsStore } from "@/stores/settings";
import { ref, onMounted, useTemplateRef, watch, computed } from "vue";

const settings = useSettingsStore();

const PASSWORD_MIN_LENGTH = 8;
const PASSWORD_MAX_LENGTH = 63;
const DEVICE_NAME_MIN_LENGTH = 8;
const DEVICE_NAME_MAX_LENGTH = 32;

const connectionModal = useTemplateRef<any>("connectionModal");
const dontShowAgain = ref(false);
const originalDeviceName = ref("");
const originalWifiPassword = ref("");
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
const originalDevicePassword = ref("");
const devicePasswordPresent = computed(() => originalDevicePassword.value !== "" && !showDevicePasswordInput.value);
const canSubmitDevicePassword = computed(() => {
  return devicePasswordDraft.value.length >= PASSWORD_MIN_LENGTH && devicePasswordDraft.value.length <= PASSWORD_MAX_LENGTH;
});
const deviceNameDraft = ref("");
const canSubmitDeviceName = computed(() => {
  return deviceNameDraft.value.length >= DEVICE_NAME_MIN_LENGTH && deviceNameDraft.value.length <= DEVICE_NAME_MAX_LENGTH;
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

function submitDeviceName() {
  if (canSubmitDeviceName.value) {    
    settings.deviceName = deviceNameDraft.value;
    markConnectionSettingsEdited();
    onPrimaryEnter();
  }
}

function submitWifiPassword() {
  if (canSubmitWifiPassword.value) {    
    settings.wifiPassword = wifiPasswordDraft.value;
    markConnectionSettingsEdited();
    onPrimaryEnter();
  }
}

function submitDevicePassword() {
  if (canSubmitDevicePassword.value) { 
    settings.devicePassword = devicePasswordDraft.value;
    markConnectionSettingsEdited();
    onPrimaryEnter();
  }
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
  deviceNameDraft.value = settings.deviceName || "";
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
  const hasConnectionChanges =
    currentDeviceName !== originalDeviceName.value ||
    currentWifiPassword !== originalWifiPassword.value;
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
    <div class="password-field">
      <label for="deviceNameInput">{{ $t('word.device_name') }}</label>
      <div class="password-row">
        <input
          id="deviceNameInput"
          v-model="deviceNameDraft"
          :placeholder="$t('description.device_name_description')"
          minlength="8"
          maxlength="35"
          pattern="^[a-zA-Z0-9_ ]{8,35}$|^$"
          name="deviceName"
          @input="markConnectionSettingsEdited"
        />
        <Button
          class="control-btn password-submit-btn"
          :class="{ 'password-submit-btn-ready': canSubmitDeviceName }"
          :disabled="!canSubmitDeviceName"
          aria-label="Enter device name"
          type="button"
          @click="submitDeviceName"
        >
          <KeyEnterIcon class="password-submit-icon" />
        </Button>
      </div>
    </div>

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
            <EyeOpenIcon v-if="showWifiPassword" key="hide" class="password-toggle-icon" />
            <EyeClosedIcon v-else key="show" class="password-toggle-icon" />
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

    <div class="password-field" v-show="settings.advancedSettingsEnabled">
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
            <EyeOpenIcon v-if="showDevicePassword" key="device-hide" class="password-toggle-icon" />
            <EyeClosedIcon v-else key="device-show" class="password-toggle-icon" />
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
  justify-content: center;
  gap: 0.5rem;
  margin-top: 1rem;
}

.connection-modal-actions {
  margin-top: 1rem;
  display: flex;
  justify-content: center;
  gap: 1rem;
}

.connection-modal-actions :deep(button.invert-hover) {
  background: var(--surface-transparent);
  color: var(--accent-color);
  border: 2px solid var(--accent-color);
}
.connection-modal-actions :deep(button.invert-hover:hover) {
  background: var(--accent-color);
  color: var(--accent-contrast-color);
  border: 2px solid var(--surface-transparent);
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
  color: var(--accent-contrast-color);
}

.password-submit-btn.password-submit-btn-ready:hover {
  background: var(--accent-color);
  border-color: var(--accent-color);
  color: var(--accent-contrast-color);
}
</style>
