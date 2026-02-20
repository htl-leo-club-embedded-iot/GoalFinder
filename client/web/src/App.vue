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
import Modal from "@/components/Modal.vue";
import {useSettingsStore} from "@/stores/settings";
import {onMounted, ref, computed} from "vue";
import {useRouter, useRoute} from "vue-router";

const settingsStore = useSettingsStore();
const connectionModal = ref<InstanceType<typeof Modal> | null>(null);
const router = useRouter();
const route = useRoute();

const isAuthPage = computed(() => route.name === 'auth');

settingsStore.$subscribe((state) => {
  if (!settingsStore.isLoading) {
    settingsStore.saveSettings();
  }
});

async function checkDeviceConnection() {
  const controller = new AbortController();
  const timeout = setTimeout(() => controller.abort(), 2000);

  try {
    const response = await fetch('/api/connection', { signal: controller.signal });
    clearTimeout(timeout);

    if (!response.ok) {
      connectionModal.value?.openDialog();
      return;
    }

    const data = await response.json();
    if (!data.success) {
      connectionModal.value?.openDialog();
    }
  } catch {
    connectionModal.value?.openDialog();
  }
}

onMounted(() => {
  settingsStore.getSettings();
  checkDeviceConnection();

  router.afterEach((to) => {
    if (to.name !== 'auth') {
      checkDeviceConnection();
    }
  });
});

</script>

<template>
  <header v-if="!isAuthPage">
    <NavigationBar/>
  </header>
  <main>
    <RouterView/>
  </main>

  <Modal ref="connectionModal" :title="$t('connection.warning_title')">
    <p>{{ $t('connection.warning_message') }}</p>
  </Modal>
</template>

<style scoped>

</style>
