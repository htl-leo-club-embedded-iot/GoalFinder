<script setup lang="ts">
import { computed } from "vue"
import { RouterLink } from "vue-router"
import StarIcon from "@/components/icons/StarIcon.vue"
import { useFavouritesStore } from "@/stores/favourites"

const props = defineProps<{
  title: string
  description: string
  to: string
  gameName: string
}>()

const favouritesStore = useFavouritesStore()
const isFavourite = computed(() => favouritesStore.isFavourite(props.gameName))

function toggleFavourite(): void {
  favouritesStore.toggleFavourite(props.gameName)
}
</script>

<template>
  <RouterLink v-slot="{ navigate }" :to="to" custom>
    <div
      class="game-card"
      :class="{ favourited: isFavourite }"
      role="button"
      tabindex="0"
      @click="() => navigate()"
      @keydown.enter="() => navigate()"
    >
      <div class="star-wrapper" @click.stop="toggleFavourite">
        <div class="favourite-btn" :class="{ favourited: isFavourite }">
          <StarIcon :filled="isFavourite" />
        </div>
      </div>
      <div class="card-content">
        <h2>{{ $t(title) }}</h2>
        <p>{{ $t(description) }}</p>
      </div>
    </div>
  </RouterLink>
</template>

<style scoped>
.game-card {
  position: relative;
  width: 100%;
  max-width: 400px;
  padding: 1rem;
  border: 2px solid var(--border-color);
  border-radius: var(--corner-radius);
  background: var(--card-background-color);
  cursor: pointer;
  margin-top: 0.5rem;
  transition: border-color 0.2s;
}

.star-wrapper {
  position: absolute;
  top: 0.5rem;
  right: 0.5rem;
}

.favourite-btn {
  display: flex;
  align-items: center;
  justify-content: center;
  width: 28px;
  height: 28px;
  border: 2px solid var(--border-color);
  border-radius: var(--corner-radius);
  color: var(--border-color);
  transition: color 0.2s, border-color 0.2s;
}

.game-card.favourited {
  border-color: gold;
}

.favourite-btn.favourited {
  color: gold;
  border-color: gold;
}

.card-content {
  margin: 0.5rem;
  text-align: center;
}

.card-content h2 {
  margin: 0;
  text-align: center;
}

.card-content p {
  margin: 0.5rem 0 0 0;
  font-size: 0.95rem;
  line-height: 1.4;
}
</style>
