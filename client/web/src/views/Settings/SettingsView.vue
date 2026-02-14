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
import Page from "@/components/Page.vue";
import VolumeUpIcon from "@/components/icons/VolumeUpIcon.vue";
import InfoCircleIcon from "@/components/icons/InfoCircleIcon.vue";
import LightbulbIcon from "@/components/icons/LightbulbIcon.vue";
import {onMounted} from "vue";
import {useSettingsStore} from "@/stores/settings";

const settingsStore = useSettingsStore();

onMounted(() => {
  settingsStore.getSettings();
});
</script>

<template>
  <Page :title="$t('header.settings')">
    <div id="settings-nav">
      <RouterLink class="settings-link" to="/settings/audio"><div><VolumeUpIcon/> {{ $t("settings.audio") }}</div></RouterLink>
      <RouterLink class="settings-link" to="/settings/devices"><div><LightbulbIcon/> LED </div></RouterLink>
      <RouterLink class="settings-link" to="/settings/system"><div><InfoCircleIcon/> {{ $t("settings.system") }}</div></RouterLink>
    </div>
    <RouterView/>
  </Page>
</template>

<style scoped>
  #settings-nav {
    display: flex;
    justify-content: center;
    overflow-x: auto;
    gap: 1.5rem;
    border-bottom: 2px solid var(--border-color);
    margin-right: -1rem;
    margin-left: -1rem;
    padding: 0 1rem 0.5rem 1rem;
    font-weight: bold;
  }

  .settings-link :hover {
    color: var(--accent-color);
    transition: 0.4s;
  }

  .settings-link > div {
    display: flex;
    gap: 0.5rem;
  }

  .router-link-active {
    color: var(--accent-color);
  }
</style>

<style>
.container {
  display: flex;
  flex-direction: column;
  align-items: center;
  width: 100%;
  padding: 1rem;
  margin: 0;
  box-sizing: border-box;
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

.button-container1 {
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

.label-container {
  display: flex;
  flex-direction: column;
  align-items: center;
  margin-bottom: 1rem;
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

.property-title {
  min-width: 150px;
}
</style>