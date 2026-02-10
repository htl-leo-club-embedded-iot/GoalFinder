<script setup lang="ts">
import { RouterView } from 'vue-router'
import NavigationBar from "@/components/NavigationBar.vue";
import {useSettingsStore} from "@/stores/settings";
import {onMounted} from "vue";

const settingsStore = useSettingsStore();

settingsStore.$subscribe((state) => {
  settingsStore.saveSettings();
});

onMounted(() => {
  settingsStore.getSettings();
});

</script>

<template>
  <header>
    <NavigationBar/>
  </header>
  <main>
    <RouterView/>
  </main>
</template>

<style scoped>

</style>
