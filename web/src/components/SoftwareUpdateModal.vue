<script setup lang="ts">

import Modal from "@/components/Modal.vue";
import UpdateIcon from "@/components/icons/UpdateIcon.vue";
import Button from "@/components/Button.vue";
import {ref, useTemplateRef} from "vue";
import {useSettingsStore} from "@/stores/settings";

const modal = useTemplateRef<typeof Modal>("modal");

const show = () => {
    modal.value?.openDialog();
};

defineExpose({show});

const settings = useSettingsStore();

const fileInput = ref<HTMLInputElement | null>(null)
let firmwareFile: File | undefined = undefined;

function onFileChanged() {
    firmwareFile = fileInput.value!.files![0];
    console.log(firmwareFile);
}

async function uploadFirmwareFile() {
    if(firmwareFile) {
        const wasSuccessful = await settings.updateFirmware(firmwareFile);

        if(wasSuccessful) {
          console.log("Successfully uploaded");
        }
    }
}

</script>

<template>
  <Modal title="Software Update" id="update-modal" ref="modal">
    <div id="update-content">
      <UpdateIcon id="update-icon"/>
      <div>
        <input type="file" ref="fileInput" @change="onFileChanged" accept=".bin">
        <Button @click="uploadFirmwareFile">Upload Firmware</Button>
      </div>
    </div>
  </Modal>
</template>

<style scoped>
  #update-modal {
    width: 40rem;
  }

  #update-content {
    padding: 10rem;
    display: flex;
    align-items: center;
    flex-direction: column;
  }

  #update-icon {
    color: cornflowerblue;
    width: 10rem;
  }
</style>