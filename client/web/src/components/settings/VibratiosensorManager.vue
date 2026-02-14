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

<template>
  <div>
    <div id="vb">
      <div class="label-container">
        <label>{{ $t("word.sensitivity") }}</label>
        <div class="button-container">
          <InputForm type="number" class="button" v-model="sensitivity" inputmode="numeric" min="0" max="100" step="1"></InputForm>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import InputForm from "@/components/InputForm.vue";
import { useSettingsStore } from "@/stores/settings";
import { useClampedValue } from "@/models/clampedValue";

const settings = useSettingsStore();

const sensitivity = useClampedValue(
  () => settings.vibrationSensorSensitivity,
  (v) => settings.vibrationSensorSensitivity = v,
  0, 100
);
</script>
