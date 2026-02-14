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
import Page from '@/components/Page.vue';
import InputForm from '@/components/InputForm.vue';
import Button from '@/components/Button.vue';

const router = useRouter();
const { t } = useI18n();

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

  const controller = new AbortController();
  const timeout = setTimeout(() => controller.abort(), 2000);

  try {
    const response = await fetch(`/api/auth?password=${encodeURIComponent(password.value)}`, {
      signal: controller.signal,
    });
    clearTimeout(timeout);

    if (!response.ok) {
      errorMessage.value = t('auth.error');
      isLoading.value = false;
      return;
    }

    const data = await response.json();

    if (data.success) {
      sessionStorage.setItem('authenticated', 'true');
      router.replace('/');
    } else if (data.timeout) {
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
</style>
