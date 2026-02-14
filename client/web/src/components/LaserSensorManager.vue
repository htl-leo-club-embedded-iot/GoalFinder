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

const settings = useSettingsStore();
</script>

<template>
  <div class="container">
    <div id="led">
      <div class="label-container">
        <label>{{ $t("word.measuring")}} {{ $t("word.distance") }}</label>
        <div class="button-container">
          <InputForm type="number" class="button" v-model="settings.ballHitDetectionDistance" inputmode="numeric" min="150" max="300" step="10"></InputForm>
        </div>
      </div>
    </div>
  </div>
</template>

<style scoped>
.container {
  display: flex;
  flex-direction: column;
  align-items: center;
  width: 100%;
  padding: 1rem;
  padding-bottom: 0.5rem !important;
  box-sizing: border-box;
}

#led {
  width: 100%;
}

.label-container {
  display: flex;
  flex-direction: column;
  align-items: center;
  margin-bottom: 1rem;
  margin-top: 0.5rem;
  width: 100%;
  gap: 0.5rem;
}

.label-container label {
  text-align: center;
  font-weight: bold;
  display: block;
}

.button-container {
  display: flex;
  justify-content: center;
  width: 100%;
}

.button {
  margin-left: 0;
}

span {
  text-align: center;
  margin-top: 0.1rem;
  font-weight: bold;
}
</style>
