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

function closeConnectionModal() {
  if (dontShowAgain.value) {
    localStorage.setItem('connectionWarningDontShow', 'true');
  }
  // update originals so repeated Enter won't reopen immediately
  originalDeviceName.value = settings.deviceName || "";
  originalWifiPassword.value = settings.wifiPassword || "";
  connectionModal.value?.closeDialog();
}

function onPrimaryEnter(field: 'device'|'wifi') {
  if (dontShowAgain.value) return;
  if (field === 'device') {
    if ((settings.deviceName || "") !== originalDeviceName.value) {
      openConnectionModal();
    }
  } else {
    if ((settings.wifiPassword || "") !== originalWifiPassword.value) {
      openConnectionModal();
    }
  }
}
</script>

<template>
  <form id="general-input" autocomplete="off">
    <InputForm v-model="settings.deviceName" :label="$t('word.device_name')"
               :placeholder="$t('description.device_name_description')" type="text"
               name="deviceName" autocomplete="off" @enter="onPrimaryEnter('device')"/>
    <InputForm v-model="settings.wifiPassword" :label="$t('word.ssid_password')"
               :placeholder="$t('description.ssid_password_description')" type="password"
               :minlength="8" :maxlength="63" pattern="^.{8,63}$|^$"
               name="wifiPassword" autocomplete="new-password" @enter="onPrimaryEnter('wifi')"/>
    <InputForm v-model="settings.devicePassword" :label="$t('word.device_password')"
               :placeholder="$t('description.device_password_description')" type="password"
               name="devicePassword" autocomplete="new-password"/>
  </form>

  <Modal ref="connectionModal" :title="$t('connection.warning_title')" centered hide-close-button>
    <p>{{ $t('connection.warning_message') }}</p>
    <label class="dont-show-again">
      <input type="checkbox" v-model="dontShowAgain" />
      {{ $t('connection.dont_show_again') }}
    </label>
    <div class="connection-modal-actions">
      <Button primary @click="closeConnectionModal">{{ $t('connection.ok') }}</Button>
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
}
</style>
