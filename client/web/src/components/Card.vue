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
  const props = defineProps([
    'title',
    'glow'
  ]);
</script>

<template>
  <div id="card" :class="glow ? 'glow' : ''">
    <slot />
  </div>
</template>

<style scoped>
  #card {
    padding: 1rem;
    border: 2px solid var(--border-color);
    border-radius: var(--corner-radius);
    background: var(--card-background-color);
  }
</style>