<script setup lang="ts">
import InputForm from "@/components/InputForm.vue";
import Modal from "@/components/Modal.vue";
import Button from "@/components/Button.vue";
import { useSettingsStore } from "@/stores/settings";
import { ref, onMounted, useTemplateRef } from "vue";

const settings = useSettingsStore();

const connectionModal = useTemplateRef<any>("connectionModal");
const dontShowAgain = ref(false);
const originalDeviceName = ref("");
const originalWifiPassword = ref("");

onMounted(() => {
  originalDeviceName.value = settings.deviceName || "";
  originalWifiPassword.value = settings.wifiPassword || "";
  const stored = localStorage.getItem('connectionWarningDontShow');
  dontShowAgain.value = stored === 'true';
});

function openConnectionModal() {
  connectionModal.value?.openDialog();
}

function closeConnectionModal(restart: boolean) {
  if (dontShowAgain.value) {
    localStorage.setItem('connectionWarningDontShow', 'true');
  }

  originalDeviceName.value = settings.deviceName || "";
  originalWifiPassword.value = settings.wifiPassword || "";
  connectionModal.value?.closeDialog();

  if (restart) {
    settings.restartDevice();
  }
}

function onPrimaryEnter() {
  if (dontShowAgain.value) {
    settings.restartDevice();
  } else if ((settings.deviceName || "") !== originalDeviceName.value || (settings.wifiPassword || "") !== originalWifiPassword.value) {
    openConnectionModal();
  }
}
</script>

<template>
  <form id="general-input" autocomplete="off">
    <InputForm v-model="settings.deviceName" :label="$t('word.device_name')"
           :placeholder="$t('description.device_name_description')" type="text"
           name="deviceName" autocomplete="off" @enter="onPrimaryEnter" @blur="onPrimaryEnter"/>
    <InputForm v-model="settings.wifiPassword" :label="$t('word.ssid_password')"
           :placeholder="$t('description.ssid_password_description')" type="password"
           :minlength="8" :maxlength="63" pattern="^.{8,63}$|^$"
           name="wifiPassword" autocomplete="new-password" @enter="onPrimaryEnter" @blur="onPrimaryEnter"/>
    <InputForm v-model="settings.devicePassword" :label="$t('word.device_password')"
               :placeholder="$t('description.device_password_description')" type="password"
               name="devicePassword" autocomplete="new-password"/>
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
  margin-top: 1rem;
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
</style>
