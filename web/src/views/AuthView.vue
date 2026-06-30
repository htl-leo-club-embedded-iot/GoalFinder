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
import { ref } from 'vue';
import { useRouter } from 'vue-router';
import { useI18n } from 'vue-i18n';
import { sha256 } from 'js-sha256';
import { useWebSocketStore } from '@/stores/websocket';
import Page from '@/components/Page.vue';
import InputForm from '@/components/InputForm.vue';
import Button from '@/components/Button.vue';
import SpectateIcon from '@/components/icons/SpectateIcon.vue';

const router = useRouter();
const { t } = useI18n();
const wsStore = useWebSocketStore();

const password = ref('');
const errorMessage = ref('');
const isLoading = ref(false);

async function authenticate() {
  if (!password.value) {
    errorMessage.value = t('auth.password_required');
    return;
  }

  isLoading.value = true;
  errorMessage.value = '';

  try {
    const passwordHash = sha256(password.value);
    const result = await wsStore.sendAuth(passwordHash);

    if (result.success) {
      sessionStorage.setItem('authenticated', 'true');
      router.replace('/');
    } else if (result.timeout) {
      errorMessage.value = t('auth.too_many_attempts');
    } else {
      errorMessage.value = t('auth.invalid_password');
    }
  } catch {
    errorMessage.value = t('auth.error');
  }

  isLoading.value = false;
}
</script>

<template>
  <Page :title="$t('auth.title')">
    <form class="auth-form" @submit.prevent="authenticate">
      <p class="auth-description">{{ $t('auth.description') }}</p>

      <InputForm
        v-model="password"
        type="password"
        :placeholder="$t('auth.password_placeholder')"
        :label="$t('word.device_password')"
      />

      <p v-if="errorMessage" class="error-message">{{ errorMessage }}</p>

      <Button type="submit" primary :disabled="isLoading">
        {{ isLoading ? '...' : $t('auth.login') }}
      </Button>
    </form>

    <div class="auth-spectate">
      <RouterLink to="/spectate?origin=auth" class="spectate-link">
        <SpectateIcon class="spectate-icon" />
        {{ $t("spectator.spectate") }}
      </RouterLink>
    </div>
  </Page>
</template>

<style scoped>
.auth-form {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 1rem;
}

.auth-description {
  text-align: center;
  color: var(--text-color-secondary);
  margin: 0;
}

.error-message {
  color: #E53935;
  text-align: center;
  margin: 0;
  font-weight: 500;
}

.auth-spectate {
  text-align: center;
  margin-top: 1.5rem;
}

.spectate-link {
  display: inline-flex;
  align-items: center;
  gap: 0.4rem;
  color: var(--text-color-secondary);
  font-size: 0.95rem;
  transition: color 0.3s;
}

.spectate-link:hover {
  color: var(--accent-color);
}

.spectate-icon {
  width: 1.2rem;
  height: 1.2rem;
}
</style>
