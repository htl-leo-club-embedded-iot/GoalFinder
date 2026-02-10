<script setup lang="ts">
  const props = defineProps([
    'title',
    'glow'
  ]);
</script>

<template>
  <div id="card" :class="glow ? 'glow' : ''">
    <slot />
  </div>
</template>

<style scoped>
  #card {
    padding: 1rem;
    border: 2px solid var(--border-color);
    border-radius: var(--corner-radius);
    background: var(--card-background-color);
  }

  /*.glow {
    box-shadow: 0 2px 8px 0 rgba(130, 130, 255, 0.3);
  } */
</style>