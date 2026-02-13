<script setup lang="ts">
  const props = defineProps({
    primary: Boolean,
    disabled: Boolean
  });
</script>

<template>
  <button :disabled="disabled" :class="primary ? 'primary' : ''"><slot/></button>
</template>

<style scoped>
  button {
    padding: 0.5rem;
    border-radius: var(--corner-radius);
    border: 2px solid var(--border-color);
    background: transparent;
    font-size: inherit;
    transition: 0.4s;
    color: var(--text-color);
  }

  button:hover {
    cursor: pointer;
    background: inherit;
    border-color: cornflowerblue;
    color: cornflowerblue;
  }

  .primary {
    background: cornflowerblue;
    color: white;
    border: 2px solid transparent;
  }

  .primary:hover {
    border: 2px solid cornflowerblue;
  }

  
</style>