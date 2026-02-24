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
import {useSettingsStore} from "@/stores/settings";
import InputForm from "@/components/InputForm.vue";
import { useClampedValue } from "@/models/clampedValue";

const settings = useSettingsStore();

const distance = useClampedValue(
  () => settings.ballHitDetectionDistance,
  (v) => settings.ballHitDetectionDistance = v,
  150, 600
);
</script>

<template>
  <div class="container">
    <div id="led">
      <div class="label-container">
        <label>{{ $t("word.measuring")}} {{ $t("word.distance") }}</label>
        <div class="button-container">
          <InputForm type="number" class="button" v-model="distance" inputmode="numeric" min="150" max="600" step="10"></InputForm>
        </div>
      </div>
    </div>
  </div>
</template>

<style scoped>
#led {
  width: 100%;
}
</style>
