<script setup lang="ts">
import {useSettingsStore} from "@/stores/settings";
import InputForm from "@/components/InputForm.vue";

const settings = useSettingsStore();
</script>

<template>
  <div>
    <div id="led">
      <div class="label-container">
        <label>{{ $t("word.measuring")}} {{ $t("word.distance") }}</label>
        <div class="button-container">
          <InputForm type="number" class="button" v-model="settings.ballHitDetectionDistance" inputmode="numeric" min="150" max="300" step="10"></InputForm>
        </div>
      </div>
    </div>
  </div>
</template>

<style scoped>
.label-container {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 1rem;
  margin-top: 0.5rem;
}

.button-container {
  display: flex;
}

.button {
  margin-left: 10px;
}

.button:first-child {
  margin-left: 0;
}

span {
  text-align: center;
  margin-top: 0.1rem;
  font-weight: bold;
}
</style>
