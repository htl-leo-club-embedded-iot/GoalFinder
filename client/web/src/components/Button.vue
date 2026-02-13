
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
  const props = defineProps({
    primary: Boolean,
    disabled: Boolean
  });
</script>

<template>
  <button :disabled="disabled" :class="primary ? 'primary' : ''"><slot/></button>
</template>

<style scoped>
  button {
    padding: 0.5rem;
    border-radius: var(--corner-radius);
    border: 2px solid var(--border-color);
    background: transparent;
    font-size: inherit;
    transition: 0.4s;
    color: var(--text-color);
  }

  button:hover {
    cursor: pointer;
    background: inherit;
    border-color: cornflowerblue;
    color: cornflowerblue;
  }

  .primary {
    background: cornflowerblue;
    color: white;
    border: 2px solid transparent;
  }

  .primary:hover {
    border: 2px solid cornflowerblue;
  }
</style>