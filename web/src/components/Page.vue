<script setup lang="ts">
  import Card from "@/components/Card.vue";

  const props = defineProps([
    'title',
  ]);
</script>

<template>
  <div id="page">
    <h1 id="page-title">{{ title }}</h1>
    <Card class="page-content" glow="true">
      <slot/>
    </Card>
  </div>
</template>

<style scoped>
#page-title {
  color: cornflowerblue;
  text-align: center;
  margin-top: 1rem;
}

/* Seite mit flexiblem, mittigem Layout */
#page {
  padding: 1rem;
  margin: 0 auto;
  max-width: 1170px;
  width: 100%;
  box-sizing: border-box;
}

/* Abstand für Inhaltskarte */
.page-content {
  margin-top: 3vh;
}

/* Für größere Bildschirme leicht mehr Abstand */
@media (min-width: 768px) {
  .page-content {
    margin-top: 5vh;
  }
}
</style>
