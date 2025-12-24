<script setup lang="ts">

import Page from "@/components/Page.vue";
</script>

<template>
  <Page :title="$t('header.about')">
    <div class="logo-holder">
      <img src="@/assets/logos/ssa_logo.svg" alt="smart sport assistance logo">
      <img src="@/assets/logos/uni_wien_logo.png" alt="uni wien logo">
      <img src="@/assets/logos/htllogo_white.png" alt="htl leonding logo">
    </div>
    <p>Dieses Gerät wurde im Zuge des "Smart Sport Assistance" Projekt in Kooperation mit Universität Wien von Schülern der HTBLA Leonding, Klasse 2BHIF, Jahrgang 23/24 entwickelt.</p>
    <p>Entwickelt von:</p>
    <ul>
      <li>Omar Al-Desoky</li>
      <li>Rabbi Baysoy</li>
      <li>Alexander Coroi</li>
      <li>Edin Kurtic</li>
    </ul>
  </Page>
</template>

<style scoped>
  .logo-holder {
    display: flex;
    flex-direction: row;
    flex-wrap: wrap;
    gap: 2rem;
    justify-content: center;
    margin: 2rem 0 5rem 0;
  }

  .logo-holder > img {
    max-width: 20rem;
    object-fit: contain;
  }
</style>