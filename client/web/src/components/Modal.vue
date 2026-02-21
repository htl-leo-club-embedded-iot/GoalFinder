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
import { useTemplateRef} from "vue";
import Button from "@/components/Button.vue";

const props = defineProps({
  title: {type: String, required: true},
  centered: {type: Boolean, default: false},
  hideCloseButton: {type: Boolean, default: false}
});

const dialog = useTemplateRef<HTMLDialogElement>("dialog");

const openDialog = () => {
  dialog.value?.showModal();
};

const closeDialog = () => {
  dialog.value?.close();
};

defineExpose({openDialog, closeDialog});

</script>

<template>
  <dialog ref="dialog" :class="{ centered }">
    <div id="dialog-header">
      <h3 id="dialog-title">{{title}}</h3>
      <Button v-if="!hideCloseButton" id="close-button" @click="closeDialog">X</Button>
    </div>
    <slot/>
  </dialog>
</template>

<style scoped>
  dialog {
    outline: none;
    border: 2px solid var(--border-color);
    border-radius: var(--corner-radius);
    margin: 2rem;
    color: var(--text-color);
    background: var(--background-color);
  }

  dialog.centered {
    margin: auto;
    text-align: center;
  }

  dialog::backdrop {
    background: rgba(0, 0, 0, 0.5);
  }

  #dialog-header {
    display: flex;
  }

  .centered #dialog-header {
    justify-content: center;
  }

  #close-button {
    width: 2rem;
    height: 2rem;
    padding: 0;
    position: relative;
    margin-left: auto;
    margin-right: 0;
    display: flex;
    align-items: center;
    justify-content: center;
  }

  #dialog-title {
    margin: 0.5rem 0 1rem 0;
  }
</style>