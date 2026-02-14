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
import Page from "@/components/Page.vue";
import Card from "@/components/Card.vue";
</script>

<template>
  <Page title="Dashboard">
    <h2>{{ $t("word.welcome_back") }}!</h2>
    <p>{{ $t("description.dashboard_description") }}</p>
    <div id="card-container">
      <Card class="quick-link">
        <div>
          <h2>{{ $t("header.games") }}</h2>
          <p>{{ $t("description.gameMode_description") }}</p>
          <RouterLink to="/games"><Button primary>{{ $t("word.open") }}</Button></RouterLink>
        </div>
      </Card>
      <Card class="quick-link">
        <div>
          <h2>{{ $t("header.settings") }}</h2>
          <p>{{ $t("description.settings_description") }}</p>
          <RouterLink to="/settings"><Button primary>{{ $t("word.open") }}</Button></RouterLink>
        </div>
      </Card>
    </div>
  </Page>
</template>

<style scoped>
  #card-container {
    gap: 0.5rem;
    display: flex;
    flex-direction: column;
    align-items: center;
    padding: 1rem;
  }

  .quick-link {
    width: 100%;
    max-width: 400px;
  }

  .quick-link h2 {
    margin: 0;
  }

  .quick-link > div {
    margin: 0.5rem;
    text-align: center;
  }

  h2 {
    text-align: center;
  }

  p {
    text-align: center;
  }
</style>
