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
import { nextTick, onMounted, onUnmounted, ref, watch } from "vue";
import Page from "@/components/Page.vue";
import { useWebSocketStore } from "@/stores/websocket";

/**
 * LogSettingsView - Streams device logs while the view is open.
 *
 * @component
 */
const wsStore = useWebSocketStore();
const logLines = ref<string[]>([]);
const logPanel = ref<HTMLElement | null>(null);
const MAX_LOG_ENTRIES = 500;

function appendLogLine(line: string): void {
  logLines.value.push(line);

  if (logLines.value.length > MAX_LOG_ENTRIES) {
    const overflow = logLines.value.length - MAX_LOG_ENTRIES;
    logLines.value.splice(0, overflow);
  }
}

function normalizeLogMessage(msg: unknown): string {
  let line = "";

  if (msg && typeof msg === "object" && "message" in msg) {
    const payload = (msg as { message?: unknown }).message;
    if (typeof payload === "string") {
      line = payload;
    } else if (payload !== null && payload !== undefined) {
      line = JSON.stringify(payload);
    }
  } else if (typeof msg === "string") {
    line = msg;
  } else if (msg !== null && msg !== undefined) {
    line = String(msg);
  }

  return line;
}

function setWebLogging(enabled: boolean): void {
  wsStore
    .sendAndWait(
      { type: "set_web_logging", value: enabled },
      "set_web_logging_ack",
      3000
    )
    .catch((error) => {
      console.warn("[LogView] Failed to toggle log relay:", error);
    });
}

function handleLogMessage(msg?: unknown): void {
  const line = normalizeLogMessage(msg);
  if (line) {
    appendLogLine(line);
  }
}

function scrollToBottom(): void {
  if (logPanel.value) {
    logPanel.value.scrollTop = logPanel.value.scrollHeight;
  }
}

watch(
  () => logLines.value.length,
  () => {
    nextTick(() => {
      scrollToBottom();
    });
  }
);

onMounted(() => {
  wsStore.on("log", handleLogMessage);
  setWebLogging(true);
});

onUnmounted(() => {
  wsStore.off("log", handleLogMessage);
  setWebLogging(false);
});
</script>

<template>
  <Page :title="$t('settings.logs')" class="page">
      <pre ref="logPanel" class="log-output">{{
        logLines.length ? logLines.join("\n") : $t("settings.logs_waiting")
      }}</pre>
  </Page>
</template>

<style scoped>
#page.page {
  margin: 0;
  max-width: none;
  padding: 0;
  width: 100%;
  display: flex;
  flex-direction: column;
  align-items: center;
}

.page :deep(.page-content) {
  margin-top: 1rem;
}

.page :deep(#card) {
  border: none;
  padding: 0;
  background: transparent;
}

.log-output {
  height: 80vh;
  width: 90vw;
  margin: 0 auto;
  background: #000000;
  color: #e6e6e6;
  font-family: "JetBrains Mono", "Fira Code", "SFMono-Regular", Menlo, Monaco,
    Consolas, "Liberation Mono", "Courier New", monospace;
  font-size: 0.85rem;
  line-height: 1.45;
  padding: 1rem;
  border-radius: var(--corner-radius);
  overflow-y: auto;
  white-space: pre-wrap;
  word-break: break-word;
}
</style>
