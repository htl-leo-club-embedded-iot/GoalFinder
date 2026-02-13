<script setup lang="ts">
import LedManager from "@/components/LedManager.vue";

</script>

<template>
  <h3>{{ $t("settings.devices") }}</h3>
  <h4>{{$t("settings.led")}}</h4>
  <LedManager/>
  
</template>

<style scoped>
</style>