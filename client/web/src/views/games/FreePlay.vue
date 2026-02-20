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
import {reactive} from "vue";
import {FreePlayGame} from "@/models/game";
import Page from "@/components/Page.vue";
import ToggleButton from "@/components/ToggleButton.vue";
import Button from "@/components/Button.vue";
import PlayIcon from "@/components/icons/PlayIcon.vue";

const game = reactive(new FreePlayGame());

function onGameStartBtnClick() {
  if (game.isRunning) {
    game.pause();
  } else {
    game.start();
  }
}

function onResetBtnClick() {
  game.reset();
}
</script>

<template>
  <Page :title="$t('games.free_play')">
    <div class="free-play">
      <div class="score-container">
        <div class="score-item">
          <span class="score-label">{{ $t("word.hits") }}</span>
          <span class="score-value hits">{{ game.hits }}</span>
        </div>
        <div class="score-item">
          <span class="score-label">{{ $t("word.misses") }}</span>
          <span class="score-value misses">{{ game.misses }}</span>
        </div>
      </div>

      <div class="buttons-container">
        <ToggleButton readonly id="game-state-button" v-model="game.isRunning" @click="onGameStartBtnClick">
          <PlayIcon id="play-icon"/>
        </ToggleButton>
        <Button @click="onResetBtnClick">{{ $t("word.restart") }}</Button>
      </div>
    </div>
  </Page>
</template>

<style scoped>
.free-play {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 2rem;
  padding: 2rem 1rem;
}

.score-container {
  display: flex;
  gap: 3rem;
  justify-content: center;
}

.score-item {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 0.5rem;
}

.score-label {
  font-size: 1.1rem;
  opacity: 0.8;
}

.score-value {
  font-size: 3rem;
  font-weight: bold;
}

.buttons-container {
  display: flex;
  gap: 1rem;
  justify-content: center;
  align-items: center;
}

#game-state-button {
  padding: 0.5rem 0.75rem;
  display: flex;
  justify-content: center;
  align-items: center;
  min-width: auto;
}

#play-icon {
  width: 1rem;
}
</style>