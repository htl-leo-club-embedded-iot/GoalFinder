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
import Checkbox from "@/components/Checkbox.vue";
import {useTemplateRef} from "vue";
import {useSettingsStore} from "@/stores/settings";
import {useRouter} from "vue-router";
import WebAppManager from "@/components/settings/WebAppManager.vue";

const settings = useSettingsStore();
const router = useRouter();

const updateDialog = useTemplateRef<typeof SoftwareUpdateModal>("updateDialog");

const openUpdateDialog = () => {
  updateDialog.value?.show();
};

const openLogs = () => {
  router.push('/settings/logs');
};
</script>

<template>
  <div class="container">
    <h3 id="web-title">{{ $t("settings.web_app") }}</h3>
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
        <div class="property-title">{{ $t("word.version") }}</div>
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

    <h3>{{ $t("word.advanced") }}</h3>
    <Checkbox v-model="settings.advancedSettingsEnabled" label="settings.advanced" @change="settings.scheduleSave()" />
    <p class="greyed">{{ $t("settings.advanced_desc") }}</p>
    <div id="logs-button-wrapper" v-show="settings.advancedSettingsEnabled">
      <Button @click="openLogs" primary>{{ $t("settings.logs") }}</Button>
    </div>

    <h3 id="visit">{{ $t("settings.info") }}</h3>
    <p id="visit-text">{{ $t("settings.more_info") }}<a href="https://goalfinder.github.io/user">{{ $t("settings.doc") }}</a></p>
  </div>
</template>

<style scoped>
h4 {
  margin: 0;
}

.greyed {
  margin-top: -0.2rem;
  text-align: center;
  color: var(--text-color-secondary);
}

#led {
  width: 100%;
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
  margin-bottom: 0;
}

#logs-button-wrapper {
  display: flex;
  flex-direction: column;
  align-items: center;
  width: 100%;
  margin-top: 0;
  top: 0;
  margin-bottom: 1rem;
}

#device-info {
  margin: 0 auto;
  display: flex;
  flex-direction: column;
  gap: 0.75rem;
  width: fit-content;
  max-width: 500px;
  margin: 0 auto;
}

#device-info > div {
  display: flex;
  flex-direction: row;
  gap: 0.75rem;
  justify-content: flex-start;
  width: auto;
}

#visit {
  margin-top: 0;
}

#visit-text {
  margin-top: 0;
  text-align: center;
}

#visit-text a {
  color: var(--accent-color);
  text-decoration: underline;
}

#web-title {
  margin-top: 0;
}

</style>