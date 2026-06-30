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
import Modal from "@/components/Modal.vue";
import UpdateIcon from "@/components/icons/UpdateIcon.vue";
import Button from "@/components/Button.vue";
import {ref, useTemplateRef} from "vue";
import {useSettingsStore} from "@/stores/settings";
import {useI18n} from "vue-i18n";

const { t } = useI18n();
const modal = useTemplateRef<typeof Modal>("modal");
const binWarningModal = useTemplateRef<typeof Modal>("binWarningModal");

const show = () => {
    modal.value?.openDialog();
    uploadProgress.value = 0;
    isUploading.value = false;
    updateSuccess.value = false;
    updateFailed.value = false;
};

defineExpose({show});

const settings = useSettingsStore();

const fileInput = ref<HTMLInputElement | null>(null)
let firmwareFile: File | undefined = undefined;
const uploadProgress = ref(0);
const isUploading = ref(false);
const updateSuccess = ref(false);
const updateFailed = ref(false);
const dontShowBinWarning = ref(false);

function onFileChanged() {
    const file = fileInput.value!.files![0];
    if (!file) return;

    firmwareFile = file;
    uploadProgress.value = 0;
    isUploading.value = false;

    if (file.name.endsWith('.bin')) {
        const hidden = localStorage.getItem('hide-bin-warning-modal');
        if (hidden !== 'true') {
            dontShowBinWarning.value = false;
            binWarningModal.value?.openDialog();
        }
    }
}

function closeBinWarning() {
    if (dontShowBinWarning.value) {
        localStorage.setItem('hide-bin-warning-modal', 'true');
    }
    binWarningModal.value?.closeDialog();
}

function uploadFirmwareFile() {
    if(!firmwareFile) return;

    isUploading.value = true;
    updateSuccess.value = false;
    updateFailed.value = false;
    uploadProgress.value = 0;

    settings.updateFirmware(
        firmwareFile,
        (percent) => {
            uploadProgress.value = percent;
        },
        () => {
            isUploading.value = false;
            updateSuccess.value = true;
        },
        () => {
            isUploading.value = false;
            updateFailed.value = true;
        }
    );
}

</script>

<template>
  <Modal title="Software Update" id="update-modal" ref="modal" centered>
    <div id="update-content">
      <UpdateIcon id="update-icon"/>

      <div v-if="!isUploading && !updateSuccess && !updateFailed" id="upload-form">
        <input type="file" ref="fileInput" @change="onFileChanged" accept=".bin,.gfpkg">
        <Button @click="uploadFirmwareFile" primary>
          {{ t('update.upload') }}
        </Button>
      </div>

      <div v-if="isUploading" id="progress-section">
        <p id="uploading-text">{{ t('update.uploading') }}</p>
        <div id="progress-bar-container">
          <div id="progress-bar" :style="{ width: uploadProgress + '%' }"></div>
        </div>
        <span id="progress-text">{{ uploadProgress }}%</span>
      </div>

      <div v-if="updateSuccess" id="result-section">
        <p id="success-text">{{ t('update.success') }}</p>
      </div>

      <div v-if="updateFailed" id="result-section">
        <p id="error-text">{{ t('update.failed') }}</p>
      </div>
    </div>
  </Modal>

  <Modal :title="t('bin_warning.title')" ref="binWarningModal" id="warning-modal" centered hide-close-button>
    <p>{{ t('bin_warning.message') }}</p>
    <label class="dont-show-again">
      <input type="checkbox" v-model="dontShowBinWarning" />
      {{ t('bin_warning.dont_show_again') }}
    </label>
    <div class="bin-warning-actions">
      <Button primary @click="closeBinWarning">{{ t('bin_warning.ok') }}</Button>
    </div>
  </Modal>
</template>

<style scoped>
  #update-modal {
    width: 40rem;
  }

  #update-content {
    padding: 2rem 3rem;
    display: flex;
    align-items: center;
    flex-direction: column;
    gap: 1.5rem;
  }

  #update-icon {
    color: var(--accent-color);
    width: 6rem;
  }

  #upload-form {
    display: flex;
    flex-direction: column;
    align-items: center;
    gap: 0.8rem;
  }

  #progress-section {
    width: 100%;
    display: flex;
    flex-direction: column;
    align-items: center;
    gap: 0.4rem;
  }

  #progress-bar-container {
    width: 100%;
    height: 1.2rem;
    background: var(--border-color, #e0e0e0);
    border-radius: 0.6rem;
    overflow: hidden;
  }

  #progress-bar {
    height: 100%;
    background: var(--accent-color);
    border-radius: 0.6rem;
    transition: width 0.2s ease;
  }

  #progress-text {
    font-size: 0.9rem;
    font-weight: bold;
  }

  #result-section {
    text-align: center;
  }

  #success-text {
    color: #4caf50;
    font-weight: bold;
  }

  #error-text {
    color: #f44336;
    font-weight: bold;
  }

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

  .bin-warning-actions {
    display: flex;
    justify-content: center;
    margin-top: 1rem;
  }

  .bin-warning-actions button {
    min-width: 5rem;
  }

  #warning-modal {
    max-width: 50vw;
  }
</style>