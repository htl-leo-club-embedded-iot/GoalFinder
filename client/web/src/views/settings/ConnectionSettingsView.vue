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
import ConnectionSettingsForm from "@/components/settings/ConnectionSettingsView.vue";
import Checkbox from "@/components/Checkbox.vue";
import {useSettingsStore} from "@/stores/settings";
import { onMounted } from "vue";

const settings = useSettingsStore();

onMounted(() => {
  const advanced_items = document.querySelectorAll(".advanced")
  console.log(advanced_items);
  advanced_items.forEach(item => {
    const element = item as HTMLElement;
    if (settings.advancedSettingsEnabled) {
      element.style.visibility = "visible";
    } else {
      element.style.visibility = "collapsed";
    }
  });
});
</script>

<template>
  <div class="container">
    <h3>{{ $t("settings.connection_general") }}</h3>
    <ConnectionSettingsForm/>

    <h3 class="advanced">{{ $t("word.advanced") }}</h3>
    <Checkbox v-model="settings.advancedSettingsEnabled" label="settings.advanced" @change="settings.scheduleSave()" class="advanced"/>
  </div>
</template>