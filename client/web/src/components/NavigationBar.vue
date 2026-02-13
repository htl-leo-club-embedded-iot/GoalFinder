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
import PowerIcon from "@/components/icons/PowerIcon.vue";
import ToggleButton from "@/components/ToggleButton.vue";
import {useSettingsStore} from "@/stores/settings";

const settings = useSettingsStore();

</script>

<template>
  <div>
    <div id="nav-bar">
      <RouterLink to="/"><h1>GoalFinder</h1></RouterLink>
      <div id="links-container">
        <RouterLink to="/games"><Button class="link">{{ $t("header.games") }}</Button></RouterLink>
        <RouterLink to="/settings"><Button class="link">{{ $t("header.settings") }}</Button></RouterLink>
        <RouterLink to="/about"><Button class="link">{{ $t("header.about") }}</Button></RouterLink>
        <ToggleButton id="power-state-button" v-model="settings.isSoundEnabled">
          <PowerIcon id="power-icon"/>
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
    color: cornflowerblue;
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

  .link {
    min-width: 6rem;
    font-weight: bold;
  }

  .router-link-active > Button {
    border-color: cornflowerblue;
  }

  #power-state-button {
    padding: 0 0.4rem 0 0.4rem;
    display: flex;
    vertical-align: center;
    justify-content: center;
  }

  #power-icon {
    width: 1.4rem;
  }
</style>