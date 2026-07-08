<script setup lang="ts">
import { computed, ref } from "vue"
import Container from "@/components/Container.vue";
import ToggleButton from "@/components/ToggleButton.vue";
import ReorderIcon from "@/components/icons/ReorderIcon.vue";
import GameCard from "@/views/games/components/GameCard.vue";
import { useFavouritesStore } from "@/stores/favourites";

interface GameData {
  title: string
  description: string
  to: string
  gameName: string
}

const games: GameData[] = [
  { title: "games.shot_challenge", description: "games.shot_challenge_desc", to: "/games/shot-challenge", gameName: "shot-challenge" },
  { title: "games.timed_shots_challenge", description: "games.timed_shots_challenge_desc", to: "/games/timed-shots-challenge", gameName: "timed-shots-challenge" },
  { title: "games.free_play", description: "games.free_play_desc", to: "/games/free-play", gameName: "free-play" },
]

const dragMode = ref(false)
const favouritesStore = useFavouritesStore()

const sortedGames = computed(() => {
  const lookup = new Map(games.map((g) => [g.gameName, g]))
  return favouritesStore.sortedGameNames.map((name) => lookup.get(name)!).filter(Boolean)
})
</script>

<template>
  <Container :title="$t('header.games')">
    <p>{{ $t("description.gameModes_description") }}</p>

    <div class="toggle-row">
      <ToggleButton v-model="dragMode"><ReorderIcon /></ToggleButton>
    </div>

    <div class="card-list">
      <GameCard
        v-for="game in sortedGames"
        :key="game.gameName"
        :title="game.title"
        :description="game.description"
        :to="game.to"
        :game-name="game.gameName"
        :drag-mode="dragMode"
      />
    </div>
  </Container>
</template>

<style>
.card-list {
  margin-top: 0.5rem;
  padding: 0 1rem;
  display: flex;
  flex-direction: column;
  align-items: center;
}

.toggle-row {
  display: flex;
  justify-content: center;
  margin: 0.5rem 0;
}

p {
  text-align: center;
  margin-bottom: 1rem;
}
</style>
