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

import VolumeManager from "@/components/VolumeManager.vue";
</script>

<template>
  <div class="container">
    <VolumeManager/>
  </div>
</template>

<style>
.container {
  display: flex;
  flex-direction: column;
  align-items: center;
  width: 100%;
  padding: 1rem;
  padding-top: 0;
  box-sizing: border-box;
}

.volume-slider-control {
  display: flex;
  flex-direction: column;
  align-items: center;
  width: 100%;
  gap: 0.5rem;
}

.volume-slider-control label {
  text-align: center;
  font-weight: bold;
  display: block;
}

.button-container {
  display: flex;
  flex-wrap: wrap;
  gap: 0.5rem;
  justify-content: center;
  width: 100%;
}

.button-container1 {
  display: flex;
  flex-wrap: wrap;
  gap: 0.5rem;
  justify-content: center;
  width: 100%;
}

.button {
  margin-left: 0;
}

.current-value {
  text-align: center;
  margin-top: 0.5rem;
  width: 100%;
}

.sound-select {
  margin-top: 1.5rem;
  display: flex;
  flex-direction: column;
  align-items: center;
  width: 100%;
  gap: 0.5rem;
}

.sound-select label {
  text-align: center;
  font-weight: bold;
}

h3 {
  margin: 0;
  margin-bottom: 1rem;
  color: var(--accent-color);
  font-size: 1.2rem;
}
</style>