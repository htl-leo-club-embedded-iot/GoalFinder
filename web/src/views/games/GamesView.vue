<script setup lang="ts">
import Container from "@/components/Container.vue";
import GameCard from "@/views/games/components/GameCard.vue";
</script>

<template>
  <Container :title="$t('header.games')">
    <p>{{ $t("description.gameModes_description") }}</p>

    <div id="card-container" class="card-list">
      <GameCard
        title="games.shot_challenge"
        description="games.shot_challenge_desc"
        to="/games/shot-challenge"
        game-name="shot-challenge"
      />
    </div>

    <div class="card-list">
      <GameCard
        title="games.timed_shots_challenge"
        description="games.timed_shots_challenge_desc"
        to="/games/timed-shots-challenge"
        game-name="timed-shots-challenge"
      />
    </div>

    <div class="card-list">
      <GameCard
        title="games.free_play"
        description="games.free_play_desc"
        to="/games/free-play"
        game-name="free-play"
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

p {
  text-align: center;
  margin-bottom: 1rem;
}
</style>
