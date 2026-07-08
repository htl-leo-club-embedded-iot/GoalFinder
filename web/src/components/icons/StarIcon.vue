<template>
  <svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" :fill="filled ? 'currentColor' : 'none'" viewBox="0 0 16 16">
    <path d="M8 1L9.6 5.8L14.7 5.8L10.6 8.8L12.1 13.7L8 10.7L3.9 13.7L5.4 8.8L1.3 5.8L6.4 5.8Z" stroke="currentColor" stroke-width="1.2" stroke-linejoin="round"/>
  </svg>
</template>

<script setup lang="ts">
defineProps<{
  filled?: boolean
}>()
</script>
