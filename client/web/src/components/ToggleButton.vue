<script setup lang="ts">
import Button from "@/components/Button.vue";

const props = defineProps({
  readonly: Boolean
});

const model = defineModel({type: Boolean});

function onBtnClick() {
  if(!props.readonly)
  {
    model.value = !model.value;
  }
}
</script>

<template>
  <Button :primary="model" @click="onBtnClick">
    <slot/>
  </Button>
</template>

<style scoped>

</style>