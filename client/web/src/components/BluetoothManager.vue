<script setup lang="ts">
import {useSettingsStore} from "@/stores/settings";
import ToggleButton from "./ToggleButton.vue";
import BluetoothIcon from "@/components/icons/BluetoothIcon.vue";

const settings = useSettingsStore();
</script>

<template>
  <div>
    <div id="bluetooth">
      <div class="label-container">
        <label for="modus">{{ $t("word.bluetooth") }}</label>
        <ToggleButton id="bluetooth-state-button" v-model="settings.isBluetoothEnabled">
            <BluetoothIcon id="icon-button"></BluetoothIcon>
        </ToggleButton>
      </div>
    </div>

    <div v-if="settings.isBluetoothEnabled" class="devices-container">
    </div>
  </div>
</template>

<style scoped>
.label-container {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 1rem;
  margin-top: 1.5rem;
}

#bluetooth-state-button {
  padding: 0.3rem 0.4rem 0.3rem 0.4rem;
}

#icon-button {
  width: 1.4rem;
}

.devices-container {
  margin-top: 1rem;
  display: flex;
  flex-direction: column;
  gap: 1rem;
}

.device-box {
  border: 1px solid gray;
  padding: 10px;
  border-radius: 5px;
  background-color: #2a2a2a;
  color: white;
}
</style>