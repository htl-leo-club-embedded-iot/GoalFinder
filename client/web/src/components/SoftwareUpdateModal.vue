<script setup lang="ts">

import Modal from "@/components/Modal.vue";
import UpdateIcon from "@/components/icons/UpdateIcon.vue";
import Button from "@/components/Button.vue";
import {ref, useTemplateRef} from "vue";
import {useSettingsStore} from "@/stores/settings";

const modal = useTemplateRef<typeof Modal>("modal");

const show = () => {
    modal.value?.openDialog();
    uploadProgress.value = 0;
    isUploading.value = false;
};

defineExpose({show});

const settings = useSettingsStore();

const fileInput = ref<HTMLInputElement | null>(null)
let firmwareFile: File | undefined = undefined;
const uploadProgress = ref(0);
const isUploading = ref(false);

function onFileChanged() {
    firmwareFile = fileInput.value!.files![0];
    uploadProgress.value = 0;
    isUploading.value = false;
}

function uploadFirmwareFile() {
    if(!firmwareFile) return;

    isUploading.value = true;
    uploadProgress.value = 0;

    settings.updateFirmware(firmwareFile, (percent) => {
        uploadProgress.value = percent;
    });
}

</script>

<template>
  <Modal title="Software Update" id="update-modal" ref="modal">
    <div id="update-content">
      <UpdateIcon id="update-icon"/>
      <div id="upload-form">
        <input type="file" ref="fileInput" @change="onFileChanged" accept=".bin">
        <Button @click="uploadFirmwareFile" :disabled="isUploading" primary>
          {{ isUploading ? 'Uploading...' : 'Upload Firmware' }}
        </Button>
      </div>

      <div v-if="isUploading" id="progress-section">
        <div id="progress-bar-container">
          <div id="progress-bar" :style="{ width: uploadProgress + '%' }"></div>
        </div>
        <span id="progress-text">{{ uploadProgress }}%</span>
      </div>
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
    color: cornflowerblue;
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
    background: cornflowerblue;
    border-radius: 0.6rem;
    transition: width 0.2s ease;
  }

  #progress-text {
    font-size: 0.9rem;
    font-weight: bold;
  }
</style>