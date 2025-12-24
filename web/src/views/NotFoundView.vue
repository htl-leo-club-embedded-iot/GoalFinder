<script setup lang="ts">

import Page from "@/components/Page.vue";
import Button from "@/components/Button.vue";
import Card from "@/components/Card.vue";
import WifiOffIcon from "@/components/icons/WifiOffIcon.vue";
</script>

<template>
  <div id="card">
    <WifiOffIcon id="no-internet-icon"/>
    <h2>404 Not Found</h2>
    <p>The Page you are looking for doesn't exist or an other error occured.</p>
    <RouterLink to="/"><Button>Go Home</Button></RouterLink>
  </div>
</template>

<style scoped>
  #card {
    text-align: center;
    margin: 20vmin;
  }

  #no-internet-icon {
    width: 10rem;
    color: cornflowerblue;
  }
</style>