<script setup lang="ts">

import VolumeManager from "@/components/VolumeManager.vue";
</script>

<template>
  <h3>{{ $t("settings.audio") }}</h3>
  <h4>{{ $t("word.volume") }}</h4>
  <VolumeManager/>
</template>

<style scoped>

</style>