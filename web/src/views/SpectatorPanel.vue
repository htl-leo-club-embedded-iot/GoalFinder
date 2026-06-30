<script setup lang="ts">
import { ref, onMounted, onUnmounted, computed } from "vue";
import { useRoute, useRouter } from "vue-router";
import { useI18n } from "vue-i18n";
import { useWebSocketStore } from "@/stores/websocket";
import type { EventCallback } from "@/stores/websocket";
import Card from "@/components/Card.vue";
import HomeIcon from "@/components/icons/HomeIcon.vue";

const route = useRoute();
const router = useRouter();
const { t } = useI18n();
const wsStore = useWebSocketStore();

const hits = ref(0);
const misses = ref(0);

const hasActivity = computed(() => hits.value > 0 || misses.value > 0);

const origin = computed(() => {
  const o = route.query.origin;
  return o === "app" ? "app" : "auth";
});

let hitHandler: EventCallback | null = null;
let missHandler: EventCallback | null = null;

function goHome() {
  const path = origin.value === "app" ? "/" : "/auth";
  router.push(path);
}

onMounted(() => {
  hitHandler = () => { hits.value++; };
  missHandler = () => { misses.value++; };
  wsStore.on("hit", hitHandler);
  wsStore.on("miss", missHandler);
});

onUnmounted(() => {
  if (hitHandler) {
    wsStore.off("hit", hitHandler);
    hitHandler = null;
  }
  if (missHandler) {
    wsStore.off("miss", missHandler);
    missHandler = null;
  }
});
</script>

<template>
  <div id="spectator-page">
    <div id="spectator-top-bar">
      <button id="home-button" @click="goHome" :title="$t('header.home')">
        <HomeIcon />
      </button>
    </div>

    <div id="spectator-content">
      <Card id="score-card" glow="true">
        <div class="score-container">
          <div class="score-item">
            <span class="score-label">{{ $t("word.hits") }}</span>
            <span class="score-value hits">{{ hits }}</span>
          </div>
          <div class="score-item">
            <span class="score-label">{{ $t("word.misses") }}</span>
            <span class="score-value misses">{{ misses }}</span>
          </div>
        </div>
      </Card>
    </div>
  </div>
</template>

<style scoped>
#spectator-page {
  height: 100dvh;
  display: flex;
  flex-direction: column;
}

#spectator-top-bar {
  flex-shrink: 0;
  padding: 0.75rem 1rem;
  display: flex;
  align-items: center;
}

#home-button {
  display: inline-flex;
  align-items: center;
  justify-content: center;
  padding: 0.5rem;
  border-radius: var(--corner-radius);
  border: 2px solid var(--border-color);
  background: transparent;
  color: var(--text-color);
  cursor: pointer;
  transition: 0.3s;
}

#home-button:hover {
  border-color: var(--accent-color);
  color: var(--accent-color);
}

#home-button svg {
  width: 1.3rem;
  height: 1.3rem;
}

#spectator-content {
  flex: 1;
  display: flex;
  align-items: center;
  justify-content: center;
  padding: 1rem;
}

#score-card {
  width: 100%;
  max-width: 400px;
}

.score-container {
  display: flex;
  gap: 3rem;
  justify-content: center;
  padding: 2rem 1rem;
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

@media (max-width: 768px) {
  .score-container {
    gap: 2rem;
  }

  .score-value {
    font-size: 2.5rem;
  }
}
</style>
