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
import WifiOffIcon from "@/components/icons/WifiOffIcon.vue";
</script>

<template>
  <div id="card">
    <WifiOffIcon id="no-internet-icon"/>
    <h2>404 Not Found</h2>
    <p>The Page you are looking for doesn't exist or an other error occurred.</p>
    <RouterLink to="/"><Button>Go Home</Button></RouterLink>
  </div>
</template>

<style scoped>
  #card {
    text-align: center;
    margin: 20vmin;
  }

  #no-internet-icon {
    width: 10rem;
    color: var(--accent-color);
  }
</style>