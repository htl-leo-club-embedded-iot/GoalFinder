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
import SoftwareUpdateModal from "@/components/settings/SoftwareUpdateModal.vue";
import Button from "@/components/Button.vue";
import {useTemplateRef} from "vue";
import {useSettingsStore} from "@/stores/settings";
import InputForm from "@/components/InputForm.vue";
import VibrationSensorManager from "@/components/settings/VibratiosensorManager.vue";
import LaserSensorManager from "@/components/settings/LaserSensorManager.vue";
import WebAppManager from "@/components/settings/WebAppManager.vue";

const settings = useSettingsStore();

const updateDialog = useTemplateRef<typeof SoftwareUpdateModal>("updateDialog");

const openUpdateDialog = () => {
  updateDialog.value?.show();
};
</script>

<template>
  <div class="container">
    <h3>{{ $t("settings.connection") }}</h3>
    <form id="general-input">
      <InputForm v-model="settings.deviceName" :label="$t('word.device_name')"
                 :placeholder="$t('description.device_name_description')" type="text"/>
      <InputForm v-model="settings.wifiPassword" :label="$t('word.ssid_password')"
                 :placeholder="$t('description.ssid_password_description')" type="password"
                 :minlength="8" :maxlength="63" pattern="^.{8,63}$|^$"/>
      <InputForm v-model="settings.devicePassword" :label="$t('word.device_password')"
                 :placeholder="$t('description.device_password_description')" type="password"/>
    </form>

    <h3 id="sensors">{{$t("settings.sensors")}}</h3>
    <VibrationSensorManager/>
    <LaserSensorManager/>

    <h3>{{ $t("settings.web_app") }}</h3>
    <WebAppManager/>
    
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

    <h3 id="visit">{{ $t("settings.info") }}</h3>
    <p id="visit-text">{{ $t("settings.more_info") }}<a href="https://goalfinder.github.io/user">{{ $t("settings.doc") }}</a></p>
  </div>
</template>

<style>
h4 {
  margin: 0.5rem 0;
}

#led {
  width: 100%;
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
  margin-top: 1rem;
}

#system-buttons {
  display: flex;
  flex-direction: column;
  gap: 0.5rem;
  align-items: center;
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

#visit {
  margin-top: 2rem;
}

#visit-text {
  margin-top: 0;
}

#visit-text a {
  color: var(--accent-color);
  text-decoration: underline;
}
</style>