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
import ConnectionSettingsView from "@/components/settings/ConnectionSettingsView.vue";
import Checkbox from "@/components/Checkbox.vue";
import {useSettingsStore} from "@/stores/settings";

const settings = useSettingsStore();
</script>

<template>
  <div class="container">
    <h3>{{ $t("settings.connection_general") }}</h3>
    <ConnectionSettingsView/>

    <div class="container" v-show="settings.advancedSettingsEnabled">  
      <h3 class="advanced">{{ $t("word.advanced") }}</h3>
      <Checkbox v-model="settings.dnsEnabled" label="settings.dnsEnable" @change="settings.scheduleSave()" class="advanced"/>
    </div>
  </div>
</template>

<style scoped>
.advanced {
  text-align: center;
}
</style>