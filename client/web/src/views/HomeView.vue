<script setup lang="ts">
import Button from "@/components/Button.vue";
import Page from "@/components/Page.vue";
import Card from "@/components/Card.vue";
</script>

<template>
  <Page title="Dashboard">
    <h2>{{ $t("word.welcome_back") }}!</h2>
    <p>{{ $t("description.dashboard_description") }}</p>
    <div id="card-container">
      <Card class="quick-link">
        <div>
          <h2>{{ $t("header.games") }}</h2>
          <p>{{ $t("description.gameMode_description") }}</p>
          <RouterLink to="/games"><Button primary>{{ $t("word.open") }}</Button></RouterLink>
        </div>
      </Card>
      <Card class="quick-link">
        <div>
          <h2>{{ $t("header.settings") }}</h2>
          <p>{{ $t("description.settings_description") }}</p>
          <RouterLink to="/settings"><Button primary>{{ $t("word.open") }}</Button></RouterLink>
        </div>
      </Card>
    </div>
  </Page>
</template>

<style scoped>
  #card-container {
    gap: 0.5rem;
    display: flex;
    flex-wrap: wrap;
  }

  .quick-link {
    width: 0;
    flex-basis: 100%;
  }

  .quick-link h2 {
    margin: 0;
  }

  .quick-link > div {
    margin: 0.5rem;
  }
</style>
