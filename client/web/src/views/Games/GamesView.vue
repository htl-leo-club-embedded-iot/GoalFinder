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
  <Page :title="$t('header.games')">
    <p>{{ $t("description.gameModes_description") }}</p>

    <div id="card-container">
      <Card class="quick-link">
        <div>
          <h2>{{ $t("header.game_shot_challenge") }}</h2>
          <p class="game-description">
            Teste deine Treffsicherheit! Du hast <strong>60 Sekunden</strong>, um zu treffen.
            Bleib im Rhythmus,
            halte den Druck aus und zeig, wer den besten Wurf hat!
          </p>
          <RouterLink to="/games/shot-challenge">
            <Button primary>{{ $t("word.open") }}</Button>
          </RouterLink>
        </div>
      </Card>
    </div>

    <div id="timed-shots-challenge">
      <Card class="quick-link">
        <div>
          <h2>{{ $t("header.game_timed_shots_challenge") }}</h2>
          <p class="game-description">
            Hier zählt jede Sekunde! In einem festen Zeitlimit heißt es:
            <strong>So viele Treffer wie möglich</strong> landen.
            Wie viele Körbe schaffst du, bevor der Countdown endet?
          </p>
          <RouterLink to="/games/timed-shots-challenge">
            <Button primary>{{ $t("word.open") }}</Button>
          </RouterLink>
        </div>
      </Card>
    </div>
  </Page>
</template>

<style>
#card-container {
  margin-bottom: 1rem;
}

#timed-shots-challenge {
  margin-top: 1rem;
}

.game-description {
  margin: 0.5rem 0 1rem 0;
  font-size: 0.95rem;
  line-height: 1.4;
}
</style>
