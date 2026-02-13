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
  <h3>{{ $t("settings.general") }}</h3>
  <h4>Info</h4>
  <form id="general-input">
    <InputForm v-model="settings.deviceName" :label="$t('word.device_name')"
               :placeholder="$t('description.device_name_description')" type="text"/>
    <InputForm v-model="settings.devicePassword" :label="$t('word.device_password')"
               :placeholder="$t('description.device_password_description')" type="password"/>
  </form>

  <h4>{{$t("settings.sensoren")}}</h4>
  <VibrationSensorManager/>
  <LaserSensorManager/>

  <h3>{{ $t("settings.system") }}</h3>
  <h4>{{ $t("settings.system") }}</h4>
  <div id="system-buttons">
    <Button @click="settings.restartDevice()" primary>{{ $t("word.restart") }}</Button>
    <Button @click="settings.factoryResetDevice()">{{ $t("word.factory_settings_reset") }}</Button>
  </div>
  <h4>Info</h4>
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
  <h4>{{ $t("word.software_update") }}</h4>
  <Button primary @click="openUpdateDialog">{{ $t("word.search_for_update") }}</Button>
  <SoftwareUpdateModal ref="updateDialog"/>
</template>

<style scoped>
#general-input {
  display: flex;
  flex-direction: column;
  gap: 1rem;
}

#system-buttons {
  display: flex;
  gap: 0.4rem;
  flex-wrap: wrap;
}

#device-info {
  margin-top: 0;
  display: flex;
  flex-direction: column;
  justify-content: space-between;
  gap: 0.3rem;
}

#device-info > div {
  display: flex;
  flex-wrap: wrap;
  flex-direction: row;
}

.property-title {
  width: 25vmin;
  font-weight: bold;
}

h4 {
  color: cornflowerblue;
}

</style>