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
import Button from "@/components/Button.vue";
import {useSettingsStore} from "@/stores/settings";
import {onMounted, ref, computed} from "vue";
import {useRouter, useRoute} from "vue-router";

const settingsStore = useSettingsStore();
const connectionModal = ref<InstanceType<typeof Modal> | null>(null);
const router = useRouter();
const route = useRoute();
const dontShowAgain = ref(false);

const isAuthPage = computed(() => route.name === 'auth');

settingsStore.$subscribe(() => {
  settingsStore.scheduleSave();
});

function showConnectionModal() {
  const hidden = localStorage.getItem('hide-connection-modal');
  if (hidden === 'true') return;
  connectionModal.value?.openDialog();
}

function closeConnectionModal() {
  if (dontShowAgain.value) {
    localStorage.setItem('hide-connection-modal', 'true');
  }
  connectionModal.value?.closeDialog();
}

async function checkDeviceConnection() {
  const controller = new AbortController();
  const timeout = setTimeout(() => controller.abort(), 5000);

  try {
    const response = await fetch('/api/connection', { signal: controller.signal });
    clearTimeout(timeout);

    if (!response.ok) {
      showConnectionModal();
      return;
    }

    const data = await response.json();
    if (!data.success) {
      showConnectionModal();
    }
  } catch {
    showConnectionModal();
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

  <Modal ref="connectionModal" :title="$t('connection.warning_title')" centered hide-close-button>
    <p>{{ $t('connection.warning_message') }}</p>
    <label class="dont-show-again">
      <input type="checkbox" v-model="dontShowAgain" />
      {{ $t('connection.dont_show_again') }}
    </label>
    <div class="connection-modal-actions">
      <Button primary @click="closeConnectionModal">{{ $t('connection.ok') }}</Button>
    </div>
  </Modal>
</template>

<style scoped>
  .dont-show-again {
    display: flex;
    align-items: center;
    justify-content: center;
    gap: 0.4rem;
    margin-top: 1rem;
    cursor: pointer;
    user-select: none;
  }

  .dont-show-again input[type="checkbox"] {
    cursor: pointer;
    width: 1rem;
    height: 1rem;
    accent-color: var(--accent-color);
  }

  .connection-modal-actions {
    display: flex;
    justify-content: center;
    margin-top: 1rem;
  }

  .connection-modal-actions button {
    min-width: 5rem;
  }
</style>
