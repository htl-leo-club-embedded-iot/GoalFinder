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
import { RouterView } from 'vue-router'
import NavigationBar from "@/components/NavigationBar.vue";
import {useSettingsStore} from "@/stores/settings";
import {onMounted} from "vue";

const settingsStore = useSettingsStore();

settingsStore.$subscribe((state) => {
  settingsStore.saveSettings();
});

onMounted(() => {
  settingsStore.getSettings();
});

</script>

<template>
  <header>
    <NavigationBar/>
  </header>
  <main>
    <RouterView/>
  </main>
</template>

<style scoped>

</style>
