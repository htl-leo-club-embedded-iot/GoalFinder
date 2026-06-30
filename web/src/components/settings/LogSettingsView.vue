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
type LogLevel = "ok" | "debug" | "info" | "warn" | "error" | "unknown";
type LogLine = { text: string; level: LogLevel };

const logLines = ref<LogLine[]>([]);
const logPanel = ref<HTMLElement | null>(null);
const MAX_LOG_ENTRIES = 500;
const isColoringEnabled = ref(true);

const LOG_LEVEL_MAP: Record<string, LogLevel> = {
  OK: "ok",
  DEBUG: "debug",
  INFO: "info",
  WARN: "warn",
  ERROR: "error",
};

function appendLogLine(line: LogLine): void {
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

function parseLogLine(line: string): LogLine {
  let level: LogLevel = "unknown";
  const match = /^\[([A-Z]+)\]/.exec(line);

  if (match && match[1]) {
    const normalized = match[1].toUpperCase();
    const mappedLevel = LOG_LEVEL_MAP[normalized];
    if (mappedLevel) {
      level = mappedLevel;
    }
  }

  return { text: line, level };
}

function lineClass(line: LogLine): string {
  let className = "log-line";

  if (isColoringEnabled.value) {
    className += ` log-line--${line.level}`;
  }

  return className;
}

function setWebLogging(enabled: boolean): void {
  wsStore
    .sendAndWait(
      { type: "set_web_logging", value: enabled },
      "set_web_logging_ack",
      3000
    )
    .catch((error) => {
      console.warn("Failed to toggle log relay:", error);
    });
}

function handleLogMessage(msg?: unknown): void {
  const line = normalizeLogMessage(msg);
  if (line) {
    appendLogLine(parseLogLine(line));
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
    <div class="log-view">
      <div ref="logPanel" class="log-output">
        <div v-if="!logLines.length" class="log-line log-line--empty">
          {{ $t("settings.logs_waiting") }}
        </div>
        <template v-else>
          <div
            v-for="(line, index) in logLines"
            :key="index"
            :class="lineClass(line)"
          >
            {{ line.text }}
          </div>
        </template>
      </div>
      <div class="log-controls">
        <label class="log-toggle">
          <input v-model="isColoringEnabled" type="checkbox" />
          {{ $t("settings.logs_coloring") }}
        </label>
      </div>
    </div>
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

.log-view {
  width: 90vw;
  margin: 0 auto;
  display: flex;
  flex-direction: column;
  align-items: stretch;
  gap: 0.75rem;
}

.log-output {
  height: 80vh;
  width: 100%;
  background: var(--log-background-color);
  color: var(--log-foreground-color);
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

.log-line {
  white-space: pre-wrap;
  color: var(--log-foreground-color);
}

.log-line--empty {
  color: var(--text-color-secondary);
}

.log-line--ok {
  color: var(--log-level-ok);
}

.log-line--debug {
  color: var(--log-level-debug);
}

.log-line--info {
  color: var(--log-level-info);
}

.log-line--warn {
  color: var(--log-level-warn);
}

.log-line--error {
  color: var(--log-level-error);
}

.log-controls {
  display: flex;
  justify-content: flex-end;
}

.log-toggle {
  display: inline-flex;
  align-items: center;
  gap: 0.5rem;
  color: var(--text-color-secondary);
  font-size: 0.9rem;
}
</style>
