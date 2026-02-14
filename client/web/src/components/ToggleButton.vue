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
import Button from "@/components/Button.vue";

const props = defineProps({
  readonly: Boolean
});

const model = defineModel({type: Boolean});

function onBtnClick() {
  if(!props.readonly)
  {
    model.value = !model.value;
  }
}
</script>

<template>
  <Button :primary="model" @click="onBtnClick">
    <slot/>
  </Button>
</template>

<style scoped>

</style>