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
  import Card from "@/components/Card.vue";

  const props = defineProps([
    'title',
  ]);
</script>

<template>
  <div id="page">
    <h1 id="page-title">{{ title }}</h1>
    <Card class="page-content" glow="true">
      <slot/>
    </Card>
  </div>
</template>

<style scoped>
#page-title {
  color: cornflowerblue;
  text-align: center;
  margin-top: 1rem;
}

/* Centered wrapped page design */
#page {
  padding: 1rem;
  margin: 0 auto;
  max-width: 1170px;
  width: 100%;
  box-sizing: border-box;
}

.page-content {
  margin-top: 3vh;
}

/* Adjustment for larger screens */
@media (min-width: 768px) {
  .page-content {
    margin-top: 5vh;
  }
}
</style>
