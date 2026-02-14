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
import SoftwareUpdateModal from "@/components/SoftwareUpdateModal.vue";
import Button from "@/components/Button.vue";
import {useTemplateRef} from "vue";
import {useSettingsStore} from "@/stores/settings";
import InputForm from "@/components/InputForm.vue";
import VibrationSensorManager from "@/components/VibratiosensorManager.vue";
import LaserSensorManager from "@/components/LaserSensorManager.vue";

const settings = useSettingsStore();

const updateDialog = useTemplateRef<typeof SoftwareUpdateModal>("updateDialog");

const openUpdateDialog = () => {
  updateDialog.value?.show();
};

function isSSIDValid(value: string) {
  const regex = /^[a-zA-Z0-9_]+$/;
  if (value == null || value.length < 3 || value.length > 32 || !regex.test(value)) {
    settings.isValid = false;
    return false;
  }
  settings.isValid = true;
  return true;
}
</script>

<template>
  <div class="container">
    <h3>{{ $t("settings.connection") }}</h3>
    <form id="general-input">
      <InputForm v-model="settings.deviceName" :label="$t('word.device_name')"
                 :placeholder="$t('description.device_name_description')" type="text"/>
      <InputForm v-model="settings.devicePassword" :label="$t('word.device_password')"
                 :placeholder="$t('description.device_password_description')" type="password"/>
    </form>

    <h3 id="sensors">{{$t("settings.sensoren")}}</h3>
    <VibrationSensorManager/>
    <LaserSensorManager/>

    <h3>{{ $t("settings.system") }}</h3>
    <div id="system-buttons">
      <Button @click="settings.restartDevice()" primary>{{ $t("word.restart") }}</Button>
      <Button @click="settings.factoryResetDevice()">{{ $t("word.factory_settings_reset") }}</Button>
    </div>
    <h3>Info</h3>
    <div id="device-info">
      <div>
        <div class="property-title">{{ $t("word.device_name") }}</div>
        <div>{{ settings.deviceName }}</div>
      </div>
      <div>
        <div class="property-title">{{ $t("word.curr_version") }}</div>
        <div>{{ settings.version }}</div>
      </div>
      <div>
        <div class="property-title">{{ $t("word.mac_address") }}</div>
        <div>{{ settings.macAddress }}</div>
      </div>
    </div>
    <h3>{{ $t("word.software_update") }}</h3>
    <Button primary @click="openUpdateDialog">{{ $t("word.search_for_update") }}</Button>
    <SoftwareUpdateModal ref="updateDialog"/>
  </div>
</template>

<style>
.container {
  display: flex;
  flex-direction: column;
  align-items: center;
  width: 100%;
  padding: 1rem;
  box-sizing: border-box;
  padding-top: 2rem;
}

.volume-slider-control {
  display: flex;
  flex-direction: column;
  align-items: center;
  width: 100%;
  gap: 0.5rem;
}

.volume-slider-control label {
  text-align: center;
  font-weight: bold;
  display: block;
}

.button-container {
  display: flex;
  flex-wrap: wrap;
  gap: 0.5rem;
  justify-content: center;
  width: 100%;
}

.button {
  margin-left: 0;
}

.current-value {
  text-align: center;
  margin-top: 0.5rem;
  font-weight: bold;
  width: 100%;
}

.sound-select {
  margin-top: 1.5rem;
  display: flex;
  flex-direction: column;
  align-items: center;
  width: 100%;
  gap: 0.5rem;
}

.sound-select label {
  text-align: center;
  font-weight: bold;
}

h3 {
  margin: 0;
  margin-bottom: 1rem;
  color: var(--accent-color);
  font-size: 1.2rem;
}

#led {
  width: 100%;
}

.label-container {
  display: flex;
  flex-direction: column;
  align-items: center;
  margin-bottom: 1rem;
  margin-top: 0.5rem;
  width: 100%;
  gap: 0.5rem;
}

.label-container label {
  text-align: center;
  font-weight: bold;
  display: block;
}

.current-mode {
  text-align: center;
  width: 100%;
}

.current-mode span {
  font-weight: bold;
}

#sensors {
  margin-top: 2rem;
}

#general-input {
  display: flex;
  flex-direction: column;
  gap: 1rem;
  width: 100%;
  max-width: 500px;
}

#system-buttons {
  display: flex;
  gap: 0.5rem;
  flex-wrap: wrap;
  justify-content: center;
  width: 100%;
  margin-bottom: 1rem;
}

#device-info {
  margin-top: 0;
  display: flex;
  flex-direction: column;
  justify-content: space-between;
  gap: 0.5rem;
  width: 100%;
  max-width: 500px;
  margin-bottom: 1rem;
}

#device-info > div {
  display: flex;
  flex-wrap: wrap;
  flex-direction: row;
  gap: 0.5rem;
  justify-content: center;
  text-align: center;
}

.property-title {
  min-width: 150px;
}
</style>