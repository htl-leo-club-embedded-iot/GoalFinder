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
import BasketballIcon from "@/components/icons/BasketballIcon.vue";
import Button from "@/components/Button.vue";
import PauseIcon from "@/components/icons/PauseIcon.vue";
import PlayIcon from "@/components/icons/PlayIcon.vue";
import ToggleButton from "@/components/ToggleButton.vue";
import {useSettingsStore} from "@/stores/settings";
import SettingsIcon from "./icons/SettingsIcon.vue";
import InfoIcon from "./icons/InfoIcon.vue";
import SpectateIcon from "./icons/SpectateIcon.vue";

const settings = useSettingsStore();

</script>

<template>
  <div>
    <div id="nav-bar">
      <RouterLink to="/"><h1>GoalFinder</h1></RouterLink>
      <div id="links-container">
        <RouterLink to="/games"><Button :title="$t('header.games')"><BasketballIcon class="nav-icon"/></Button></RouterLink>
        <RouterLink to="/settings"><Button :title="$t('header.settings')"><SettingsIcon class="nav-icon"/></Button></RouterLink>
        <RouterLink to="/spectate?origin=app"><Button :title="$t('header.spectate')"><SpectateIcon class="nav-icon"/></Button></RouterLink>
        <RouterLink to="/about"><Button :title="$t('header.about')"><InfoIcon class="nav-icon"/></Button></RouterLink>
        <ToggleButton id="power-state-button" v-model="settings.isSoundEnabled">
          <Transition name="icon-fade" mode="out-in">
            <PlayIcon v-if="settings.isSoundEnabled" key="play" class="power-icon"/>
            <PauseIcon v-else key="pause" class="power-icon"/>
          </Transition>
        </ToggleButton>
      </div>
    </div>
  </div>
</template>

<style scoped>
  #nav-bar {
    z-index: 1;
    position: sticky;
    top: 0;
    display: flex;
    justify-content: center;
    align-items: center;
    backdrop-filter: blur(30px) saturate(120%);
    -webkit-backdrop-filter: blur(30px) saturate(120%);
    padding: 0.3rem 0 0.3rem 0;
    border-bottom: 2px solid var(--border-color);
  }

  h1 {
    margin: 0.5rem;
    color: var(--accent-color);
    display: none;
  }

  @media (min-width: 768px) {
    h1 {
      display: block;
    }
  }

  #links-container {
    margin-left: 1rem;
    display: flex;
    gap: 0.5rem;
  }

  .router-link-active > Button {
    border-color: var(--accent-color);
  }

  .nav-icon {
    width: 1.3rem;
    height: 1.3rem;
    display: block;
  }

  #power-state-button {
    padding: 0 0.4rem 0 0.4rem;
    display: flex;
    vertical-align: center;
    justify-content: center;
  }

  .power-icon {
    width: 1.4rem;
  }

  .icon-fade-enter-active,
  .icon-fade-leave-active {
    transition: opacity 0.15s ease;
  }

  .icon-fade-enter-from,
  .icon-fade-leave-to {
    opacity: 0;
  }
</style>