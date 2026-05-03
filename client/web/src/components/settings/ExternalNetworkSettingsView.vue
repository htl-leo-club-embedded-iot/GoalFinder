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
import { computed, onMounted, ref, useTemplateRef, watch } from "vue";
import Checkbox from "@/components/Checkbox.vue";
import Button from "@/components/Button.vue";
import Modal from "@/components/Modal.vue";
import { useSettingsStore } from "@/stores/settings";
import NetworkTextField from "@/components/settings/network/NetworkTextField.vue";
import NetworkTextareaField from "@/components/settings/network/NetworkTextareaField.vue";
import RemovableSecretInput from "@/components/settings/network/RemovableSecretInput.vue";
import AuthModeSelector from "@/components/settings/network/AuthModeSelector.vue";

const showAdvancedEnterpriseOptions = ref("");

const showAdvancedEnterpriseOptionsEnabled = computed<boolean>({
  get: () => showAdvancedEnterpriseOptions.value !== "",
  set: (value: boolean) => {
    showAdvancedEnterpriseOptions.value = value ? "enabled" : "";
  },
});

const settings = useSettingsStore();

const PASSWORD_MIN_LENGTH = 8;
const PASSWORD_MAX_LENGTH = 63;
const CONNECTION_WARNING_STORAGE_KEY = "connectionWarningDontShow";

const AUTH_MODE_OPEN = "open";
const AUTH_MODE_WPA2_PERSONAL = "wpa2-personal";
const AUTH_MODE_WPA2_ENTERPRISE = "wpa2-enterprise";

const PHASE2_AUTO = "auto";
const PHASE2_MSCHAPV2 = "mschapv2";
const PHASE2_GTC = "gtc";

interface ModalRefHandle {
  openDialog: () => void;
  closeDialog: () => void;
}

interface ExternalNetworkSnapshot {
  useExternalNetwork: boolean;
  authMode: string;
  externalNetworkSsid: string;
  externalNetworkUseDhcp: boolean;
  externalNetworkIp: string;
  externalNetworkSubnetMask: string;
  externalNetworkDefaultGateway: string;
  externalNetworkDnsIp: string;
  externalNetworkPassword: string;
}

interface EnterpriseSnapshot {
  identity: string;
  username: string;
  anonymousIdentity: string;
  password: string;
  phase2Method: string;
  caCertificate: string;
  clientCertificate: string;
  clientPrivateKey: string;
}

const authModeOptions = [
  { value: AUTH_MODE_OPEN, label: "settings.network_auth_open" },
  { value: AUTH_MODE_WPA2_PERSONAL, label: "settings.network_auth_personal" },
  { value: AUTH_MODE_WPA2_ENTERPRISE, label: "settings.network_auth_enterprise" },
];

const enterprisePhase2Options = [
  { value: PHASE2_AUTO, label: "settings.enterprise_phase2_auto" },
  { value: PHASE2_MSCHAPV2, label: "settings.enterprise_phase2_mschapv2" },
  { value: PHASE2_GTC, label: "settings.enterprise_phase2_gtc" },
];

const networkModal = useTemplateRef<ModalRefHandle>("networkModal");
const dontShowAgain = ref(false);
const hasUserEditedNetworkSettings = ref(false);

const externalNetworkEnabledDraft = ref(false);
const externalNetworkSsidDraft = ref("");
const externalNetworkUseDhcpDraft = ref(true);
const externalNetworkIpDraft = ref("");
const externalNetworkSubnetMaskDraft = ref("");
const externalNetworkDefaultGatewayDraft = ref("");
const externalNetworkDnsIpDraft = ref("");

const externalNetworkPasswordDraft = ref("");
const externalNetworkPasswordStoredPresent = ref(false);

const authModeDraft = ref(AUTH_MODE_WPA2_PERSONAL); 

const enterpriseIdentityDraft = ref("");
const enterpriseUsernameDraft = ref("");
const enterpriseAnonymousIdentityDraft = ref("");
const enterprisePasswordDraft = ref("");
const enterprisePasswordStoredPresent = ref(false);
const enterprisePhase2MethodDraft = ref(PHASE2_AUTO);
const enterpriseCaCertificateDraft = ref("");
const enterpriseClientCertificateDraft = ref("");
const enterpriseClientPrivateKeyDraft = ref("");

const originalExternalSnapshot = ref<ExternalNetworkSnapshot>({
  useExternalNetwork: false,
  authMode: AUTH_MODE_WPA2_PERSONAL,
  externalNetworkSsid: "",
  externalNetworkUseDhcp: true,
  externalNetworkIp: "",
  externalNetworkSubnetMask: "",
  externalNetworkDefaultGateway: "",
  externalNetworkDnsIp: "",
  externalNetworkPassword: "",
});

const originalEnterpriseSnapshot = ref<EnterpriseSnapshot>({
  identity: "",
  username: "",
  anonymousIdentity: "",
  password: "",
  phase2Method: PHASE2_AUTO,
  caCertificate: "",
  clientCertificate: "",
  clientPrivateKey: "",
});

const isPersonalAuthMode = computed(() => authModeDraft.value === AUTH_MODE_WPA2_PERSONAL);
const isEnterpriseAuthMode = computed(() => authModeDraft.value === AUTH_MODE_WPA2_ENTERPRISE);

const hasValidExternalPasswordDraft = computed(() => {
  return externalNetworkPasswordDraft.value.length >= PASSWORD_MIN_LENGTH
    && externalNetworkPasswordDraft.value.length <= PASSWORD_MAX_LENGTH;
});

const canUseSecuredPassword = computed(() => {
  return externalNetworkPasswordStoredPresent.value || hasValidExternalPasswordDraft.value;
});

const hasValidEnterprisePasswordDraft = computed(() => {
  return enterprisePasswordDraft.value.length >= PASSWORD_MIN_LENGTH
    && enterprisePasswordDraft.value.length <= PASSWORD_MAX_LENGTH;
});

const canUseEnterprisePassword = computed(() => {
  return enterprisePasswordStoredPresent.value || hasValidEnterprisePasswordDraft.value;
});

const showManualFields = computed(() => {
  return !externalNetworkUseDhcpDraft.value && settings.advancedSettingsEnabled;
});

function createExternalSnapshotFromDrafts(): ExternalNetworkSnapshot {
  const snapshot: ExternalNetworkSnapshot = {
    useExternalNetwork: externalNetworkEnabledDraft.value,
    authMode: authModeDraft.value,
    externalNetworkSsid: externalNetworkSsidDraft.value,
    externalNetworkUseDhcp: externalNetworkUseDhcpDraft.value,
    externalNetworkIp: externalNetworkIpDraft.value,
    externalNetworkSubnetMask: externalNetworkSubnetMaskDraft.value,
    externalNetworkDefaultGateway: externalNetworkDefaultGatewayDraft.value,
    externalNetworkDnsIp: externalNetworkDnsIpDraft.value,
    externalNetworkPassword: externalNetworkPasswordDraft.value,
  };
  return snapshot;
}

function createEnterpriseSnapshotFromDrafts(): EnterpriseSnapshot {
  const snapshot: EnterpriseSnapshot = {
    identity: enterpriseIdentityDraft.value,
    username: enterpriseUsernameDraft.value,
    anonymousIdentity: enterpriseAnonymousIdentityDraft.value,
    password: enterprisePasswordDraft.value,
    phase2Method: enterprisePhase2MethodDraft.value,
    caCertificate: enterpriseCaCertificateDraft.value,
    clientCertificate: enterpriseClientCertificateDraft.value,
    clientPrivateKey: enterpriseClientPrivateKeyDraft.value,
  };
  return snapshot;
}

function hasExternalSnapshotChanged(current: ExternalNetworkSnapshot, baseline: ExternalNetworkSnapshot): boolean {
  const changed =
    current.useExternalNetwork !== baseline.useExternalNetwork
    || current.authMode !== baseline.authMode
    || current.externalNetworkSsid !== baseline.externalNetworkSsid
    || current.externalNetworkUseDhcp !== baseline.externalNetworkUseDhcp
    || current.externalNetworkIp !== baseline.externalNetworkIp
    || current.externalNetworkSubnetMask !== baseline.externalNetworkSubnetMask
    || current.externalNetworkDefaultGateway !== baseline.externalNetworkDefaultGateway
    || current.externalNetworkDnsIp !== baseline.externalNetworkDnsIp
    || current.externalNetworkPassword !== baseline.externalNetworkPassword;
  return changed;
}

function hasEnterpriseSnapshotChanged(current: EnterpriseSnapshot, baseline: EnterpriseSnapshot): boolean {
  const changed =
    current.identity !== baseline.identity
    || current.username !== baseline.username
    || current.anonymousIdentity !== baseline.anonymousIdentity
    || current.password !== baseline.password
    || current.phase2Method !== baseline.phase2Method
    || current.caCertificate !== baseline.caCertificate
    || current.clientCertificate !== baseline.clientCertificate
    || current.clientPrivateKey !== baseline.clientPrivateKey;
  return changed;
}

const hasSupportedPendingChanges = computed(() => {
  const current = createExternalSnapshotFromDrafts();
  return hasExternalSnapshotChanged(current, originalExternalSnapshot.value);
});

const hasEnterprisePendingChanges = computed(() => {
  const current = createEnterpriseSnapshotFromDrafts();
  return hasEnterpriseSnapshotChanged(current, originalEnterpriseSnapshot.value);
});

const hasNetworkPendingChanges = computed(() => {
  return hasSupportedPendingChanges.value || hasEnterprisePendingChanges.value;
});

const canApplyNetworkConfiguration = computed(() => {
  let canApply = hasNetworkPendingChanges.value;

  if (canApply && externalNetworkEnabledDraft.value) {
    canApply = externalNetworkSsidDraft.value.trim().length > 0;

    if (canApply && isPersonalAuthMode.value) {
      canApply = canUseSecuredPassword.value;
    }

    if (canApply && isEnterpriseAuthMode.value) {
      const hasUsername = enterpriseUsernameDraft.value.trim().length > 0;
      canApply = hasUsername && canUseEnterprisePassword.value;
    }
  }

  return canApply;
});

function syncDraftsFromSettings(): void {
  externalNetworkEnabledDraft.value = settings.useExternalNetwork;
  externalNetworkSsidDraft.value = settings.externalNetworkSsid;
  externalNetworkUseDhcpDraft.value = settings.externalNetworkUseDhcp;
  externalNetworkIpDraft.value = settings.externalNetworkIp;
  externalNetworkSubnetMaskDraft.value = settings.externalNetworkSubnetMask;
  externalNetworkDefaultGatewayDraft.value = settings.externalNetworkDefaultGateway;
  externalNetworkDnsIpDraft.value = settings.externalNetworkDnsIp;
  externalNetworkPasswordDraft.value = settings.externalNetworkPassword;
  externalNetworkPasswordStoredPresent.value = settings.externalNetworkPassword !== "";

  if (settings.externalNetworkAuthMode === AUTH_MODE_OPEN
    || settings.externalNetworkAuthMode === AUTH_MODE_WPA2_PERSONAL
    || settings.externalNetworkAuthMode === AUTH_MODE_WPA2_ENTERPRISE
  ) {
    authModeDraft.value = settings.externalNetworkAuthMode;
  } else {
    authModeDraft.value = AUTH_MODE_WPA2_PERSONAL;
  }

  enterpriseIdentityDraft.value = settings.externalNetworkEnterpriseIdentity;
  enterpriseUsernameDraft.value = settings.externalNetworkEnterpriseUsername;
  enterpriseAnonymousIdentityDraft.value = settings.externalNetworkEnterpriseAnonymousIdentity;
  enterprisePasswordDraft.value = settings.externalNetworkEnterprisePassword;
  enterprisePasswordStoredPresent.value = settings.externalNetworkEnterprisePassword !== "";
  enterprisePhase2MethodDraft.value = settings.externalNetworkEnterprisePhase2Method;
  enterpriseCaCertificateDraft.value = settings.externalNetworkEnterpriseCaCertificate;
  enterpriseClientCertificateDraft.value = settings.externalNetworkEnterpriseClientCertificate;
  enterpriseClientPrivateKeyDraft.value = settings.externalNetworkEnterpriseClientPrivateKey;
}

function updateOriginalSnapshots(): void {
  originalExternalSnapshot.value = createExternalSnapshotFromDrafts();
  originalEnterpriseSnapshot.value = createEnterpriseSnapshotFromDrafts();
}

function markNetworkSettingsEdited(): void {
  hasUserEditedNetworkSettings.value = true;
}

function onExternalPasswordUpdated(value: string): void {
  externalNetworkPasswordDraft.value = value;
  markNetworkSettingsEdited();
}

function onExternalPasswordCommitted(): void {
  externalNetworkPasswordStoredPresent.value = true;
  markNetworkSettingsEdited();
}

function onExternalPasswordCleared(): void {
  externalNetworkPasswordStoredPresent.value = false;
  markNetworkSettingsEdited();
}

function onEnterprisePasswordUpdated(value: string): void {
  enterprisePasswordDraft.value = value;
  markNetworkSettingsEdited();
}

function onEnterprisePasswordCommitted(): void {
  enterprisePasswordStoredPresent.value = true;
  markNetworkSettingsEdited();
}

function onEnterprisePasswordCleared(): void {
  enterprisePasswordStoredPresent.value = false;
  markNetworkSettingsEdited();
}

function onAuthModeSelected(mode: string): void {
  authModeDraft.value = mode;

  if (authModeDraft.value === AUTH_MODE_OPEN) {
    externalNetworkPasswordDraft.value = "";
    externalNetworkPasswordStoredPresent.value = false;
  }

  markNetworkSettingsEdited();
}

function applyDraftsToStore(): void {
  settings.useExternalNetwork = externalNetworkEnabledDraft.value;
  settings.externalNetworkSsid = externalNetworkSsidDraft.value;
  settings.externalNetworkUseDhcp = externalNetworkUseDhcpDraft.value;
  settings.externalNetworkIp = externalNetworkIpDraft.value;
  settings.externalNetworkSubnetMask = externalNetworkSubnetMaskDraft.value;
  settings.externalNetworkDefaultGateway = externalNetworkDefaultGatewayDraft.value;
  settings.externalNetworkDnsIp = externalNetworkDnsIpDraft.value;

  if (authModeDraft.value === AUTH_MODE_OPEN) {
    settings.externalNetworkPassword = "";
  } else if (authModeDraft.value === AUTH_MODE_WPA2_PERSONAL) {
    settings.externalNetworkPassword = externalNetworkPasswordDraft.value;
  }

  settings.externalNetworkAuthMode = authModeDraft.value;
  settings.externalNetworkEnterpriseIdentity = enterpriseIdentityDraft.value;
  settings.externalNetworkEnterpriseUsername = enterpriseUsernameDraft.value;
  settings.externalNetworkEnterpriseAnonymousIdentity = enterpriseAnonymousIdentityDraft.value;
  settings.externalNetworkEnterprisePassword = enterprisePasswordDraft.value;
  settings.externalNetworkEnterprisePhase2Method = enterprisePhase2MethodDraft.value;
  settings.externalNetworkEnterpriseCaCertificate = enterpriseCaCertificateDraft.value;
  settings.externalNetworkEnterpriseClientCertificate = enterpriseClientCertificateDraft.value;
  settings.externalNetworkEnterpriseClientPrivateKey = enterpriseClientPrivateKeyDraft.value;
}

function openNetworkModal(): void {
  networkModal.value?.openDialog();
}

function persistDontShowAgainPreference(): void {
  if (dontShowAgain.value) {
    localStorage.setItem(CONNECTION_WARNING_STORAGE_KEY, "true");
  }
}

function onApplyNetworkConfiguration(): void {
  if (canApplyNetworkConfiguration.value) {
    applyDraftsToStore();

    if (hasNetworkPendingChanges.value) {
      if (dontShowAgain.value) {
        persistDontShowAgainPreference();
        updateOriginalSnapshots();
        hasUserEditedNetworkSettings.value = false;
        settings.restartDevice();
      } else {
        openNetworkModal();
      }
    } else {
      hasUserEditedNetworkSettings.value = false;
    }
  }
}

function closeNetworkModal(restart: boolean): void {
  persistDontShowAgainPreference();
  updateOriginalSnapshots();
  hasUserEditedNetworkSettings.value = false;
  networkModal.value?.closeDialog();

  if (restart) {
    settings.restartDevice();
  } else {
    settings.scheduleSave();
  }
}

onMounted(() => {
  syncDraftsFromSettings();
  updateOriginalSnapshots();

  const stored = localStorage.getItem(CONNECTION_WARNING_STORAGE_KEY);
  dontShowAgain.value = stored === "true";
});

watch(
  () => [
    settings.useExternalNetwork,
    settings.externalNetworkSsid,
    settings.externalNetworkUseDhcp,
    settings.externalNetworkIp,
    settings.externalNetworkSubnetMask,
    settings.externalNetworkDefaultGateway,
    settings.externalNetworkDnsIp,
    settings.externalNetworkPassword,
    settings.externalNetworkAuthMode,
    settings.externalNetworkEnterpriseIdentity,
    settings.externalNetworkEnterpriseUsername,
    settings.externalNetworkEnterpriseAnonymousIdentity,
    settings.externalNetworkEnterprisePassword,
    settings.externalNetworkEnterprisePhase2Method,
    settings.externalNetworkEnterpriseCaCertificate,
    settings.externalNetworkEnterpriseClientCertificate,
    settings.externalNetworkEnterpriseClientPrivateKey,
  ],
  () => {
    if (!hasUserEditedNetworkSettings.value) {
      syncDraftsFromSettings();
      updateOriginalSnapshots();
    }
  },
  { immediate: true }
);
</script>

<template>
  <div class="external-network-settings">
    <Checkbox
      v-model="externalNetworkEnabledDraft"
      label="settings.use_external_network"
      @change="markNetworkSettingsEdited"
    />

    <div class="external-network-content" v-show="externalNetworkEnabledDraft">
      <NetworkTextField
        v-model="externalNetworkSsidDraft"
        :label="$t('settings.external_network_ssid')"
        :placeholder="$t('description.external_network_ssid_description')"
        name="externalNetworkSsid"
        autocomplete="off"
        @change="markNetworkSettingsEdited"
      />

      <AuthModeSelector
        v-model="authModeDraft"
        :label="$t('settings.network_auth_mode')"
        :options="authModeOptions"
        @change="onAuthModeSelected"
      />

      <RemovableSecretInput
        v-show="isPersonalAuthMode"
        id="externalNetworkPasswordInput"
        :model-value="externalNetworkPasswordDraft"
        :label="$t('settings.external_network_password')"
        :placeholder="$t('description.external_network_password_description')"
        :stored-value-present="externalNetworkPasswordStoredPresent"
        :remove-label="$t('settings.remove_password')"
        :show-label="$t('word.show')"
        :hide-label="$t('word.hide')"
        :submit-aria-label="$t('settings.submit_secret')"
        :show-submit-button="false"
        input-name="externalNetworkPassword"
        autocomplete="new-password"
        @update:modelValue="onExternalPasswordUpdated"
        @commit="onExternalPasswordCommitted"
        @clear="onExternalPasswordCleared"
      />

      <div class="advanced-section" v-show="isEnterpriseAuthMode">
        <h4>{{ $t("settings.enterprise_configuration") }}</h4>

        <div class="enterprise-fields">
          <NetworkTextField
            v-model="enterpriseIdentityDraft"
            v-show="showAdvancedEnterpriseOptionsEnabled"
            :label="$t('settings.enterprise_identity')"
            :placeholder="$t('description.enterprise_identity_description')"
            name="enterpriseIdentity"
            autocomplete="off"
            @change="markNetworkSettingsEdited"
          />

          <NetworkTextField
            v-model="enterpriseUsernameDraft"
            :label="$t('settings.enterprise_username')"
            :placeholder="$t('description.enterprise_username_description')"
            name="enterpriseUsername"
            autocomplete="off"
            @change="markNetworkSettingsEdited"
          />

          <NetworkTextField
            v-model="enterpriseAnonymousIdentityDraft"
            v-show="showAdvancedEnterpriseOptionsEnabled"
            :label="$t('settings.enterprise_anonymous_identity')"
            :placeholder="$t('description.enterprise_anonymous_identity_description')"
            name="enterpriseAnonymousIdentity"
            autocomplete="off"
            @change="markNetworkSettingsEdited"
          />

          <RemovableSecretInput
            id="enterprisePasswordInput"
            :model-value="enterprisePasswordDraft"
            :label="$t('settings.enterprise_password')"
            :placeholder="$t('description.enterprise_password_description')"
            :stored-value-present="enterprisePasswordStoredPresent"
            :remove-label="$t('settings.remove_password')"
            :show-label="$t('word.show')"
            :hide-label="$t('word.hide')"
            :submit-aria-label="$t('settings.submit_secret')"
            :show-submit-button="false"
            input-name="enterprisePassword"
            autocomplete="new-password"
            @update:modelValue="onEnterprisePasswordUpdated"
            @commit="onEnterprisePasswordCommitted"
            @clear="onEnterprisePasswordCleared"
          />

          <AuthModeSelector
            v-model="enterprisePhase2MethodDraft"
            v-show="showAdvancedEnterpriseOptionsEnabled"
            :label="$t('settings.enterprise_phase2_method')"
            :options="enterprisePhase2Options"
            @change="markNetworkSettingsEdited"
          />

          <NetworkTextareaField
            v-model="enterpriseCaCertificateDraft"
            v-show="showAdvancedEnterpriseOptionsEnabled"
            :label="$t('settings.enterprise_ca_certificate')"
            :placeholder="$t('description.enterprise_ca_certificate_description')"
            @change="markNetworkSettingsEdited"
          />

          <NetworkTextareaField
            v-model="enterpriseClientCertificateDraft"
            v-show="showAdvancedEnterpriseOptionsEnabled"
            :label="$t('settings.enterprise_client_certificate')"
            :placeholder="$t('description.enterprise_client_certificate_description')"
            @change="markNetworkSettingsEdited"
          />

          <NetworkTextareaField
            v-model="enterpriseClientPrivateKeyDraft"
            v-show="showAdvancedEnterpriseOptionsEnabled"
            :label="$t('settings.enterprise_client_private_key')"
            :placeholder="$t('description.enterprise_client_private_key_description')"
            @change="markNetworkSettingsEdited"
          />

          <label class="enterprise-options-checkbox">
            <input type="checkbox" v-model="showAdvancedEnterpriseOptionsEnabled" />
            {{ $t('settings.advancedEnterpriseSettings') }}
          </label>
        </div>
      </div>

      <Checkbox
        v-model="externalNetworkUseDhcpDraft"
        label="settings.use_dhcp"
        @change="markNetworkSettingsEdited"
      />

      <div class="advanced-section" v-show="!externalNetworkUseDhcpDraft">
        <h4>{{ $t("settings.manual_network_configuration") }}</h4>

        <div class="manual-network-fields" v-show="showManualFields">
          <NetworkTextField
            v-model="externalNetworkIpDraft"
            :label="$t('settings.network_ip_address')"
            :placeholder="$t('description.network_ip_address_description')"
            name="externalNetworkIp"
            autocomplete="off"
            inputmode="decimal"
            @change="markNetworkSettingsEdited"
          />

          <NetworkTextField
            v-model="externalNetworkDefaultGatewayDraft"
            :label="$t('settings.network_default_gateway')"
            :placeholder="$t('description.network_default_gateway_description')"
            name="externalNetworkGateway"
            autocomplete="off"
            inputmode="decimal"
            @change="markNetworkSettingsEdited"
          />

          <NetworkTextField
            v-model="externalNetworkSubnetMaskDraft"
            :label="$t('settings.network_subnet_mask')"
            :placeholder="$t('description.network_subnet_mask_description')"
            name="externalNetworkSubnetMask"
            autocomplete="off"
            inputmode="decimal"
            @change="markNetworkSettingsEdited"
          />

          <NetworkTextField
            v-model="externalNetworkDnsIpDraft"
            :label="$t('settings.network_dns')"
            :placeholder="$t('description.network_dns_description')"
            name="externalNetworkDnsIp"
            autocomplete="off"
            inputmode="decimal"
            @change="markNetworkSettingsEdited"
          />
        </div>
      </div>
    </div>

    <div class="apply-actions" v-show="hasNetworkPendingChanges">
      <Button
        primary
        :disabled="!canApplyNetworkConfiguration"
        @click="onApplyNetworkConfiguration"
      >
        {{ $t("settings.apply_network_configuration") }}
      </Button>
    </div>
  </div>

  <Modal ref="networkModal" :title="$t('settings.restart_title')" centered hide-close-button>
    <p>{{ $t('settings.restart_desc') }}</p>
    <label class="dont-show-again">
      <input type="checkbox" v-model="dontShowAgain" />
      {{ $t('settings.restart_always') }}
    </label>
    <div class="connection-modal-actions">
      <Button primary @click="closeNetworkModal(true)">{{ $t('settings.restart_now') }}</Button>
      <Button primary class="invert-hover" @click="closeNetworkModal(false)">{{ $t('settings.restart_later') }}</Button>
    </div>
  </Modal>
</template>

<style scoped>
.external-network-settings {
  display: flex;
  flex-direction: column;
  gap: 1rem;
  width: 100%;
  max-width: 500px;
}

.external-network-content {
  display: flex;
  flex-direction: column;
  gap: 1rem;
  width: 100%;
}

.advanced-section {
  display: flex;
  flex-direction: column;
  gap: 0.6rem;
  width: 100%;
  padding: 0.75rem;
  border-radius: var(--corner-radius);
  border: 2px solid var(--border-color);
  background: var(--card-background-color);
}

.advanced-section > h4 {
  text-align: center;
  margin: 0;
}

.enterprise-fields,
.manual-network-fields {
  display: flex;
  flex-direction: column;
  gap: 0.8rem;
  width: 100%;
}

.description-text {
  text-align: center;
  color: var(--text-color-secondary);
  margin: 0;
  font-size: 0.85rem;
}

.apply-actions {
  display: flex;
  justify-content: center;
}

.dont-show-again {
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 0.5rem;
  margin-top: 1rem;
}

.enterprise-options-checkbox {
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 0.5rem;
  margin-top: 1rem;
  color: var(--text-color);
}

.enterprise-options-checkbox input[type="checkbox"] {
  accent-color: var(--accent-color);
  cursor: pointer;
}

.connection-modal-actions {
  margin-top: 1rem;
  display: flex;
  justify-content: center;
  gap: 1rem;
}

.connection-modal-actions :deep(button.invert-hover) {
  background: var(--surface-transparent);
  color: var(--accent-color);
  border: 2px solid var(--accent-color);
}

.connection-modal-actions :deep(button.invert-hover:hover) {
  background: var(--accent-color);
  color: var(--accent-contrast-color);
  border: 2px solid var(--surface-transparent);
}
</style>
