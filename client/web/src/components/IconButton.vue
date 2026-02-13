<script setup lang="ts">
const props = defineProps({
  primary: Boolean,
  danger: Boolean,
  warning: Boolean,
  title: String
});
</script>

<template>
  <button
    class="icon-btn"
    :class="{
      primary,
      danger,
      warning
    }"
    :title="title"
  >
    <slot />
  </button>
</template>

<style scoped>
.icon-btn {
  display: inline-flex;
  align-items: center;
  justify-content: center;
  width: 2.4rem;
  height: 2.4rem;
  border-radius: 50%;
  border: 2px solid var(--border-color);
  background: transparent;
  color: var(--text-color);
  transition: 0.3s;
}

.icon-btn:hover {
  cursor: pointer;
  border-color: cornflowerblue;
  color: cornflowerblue;
}

.icon-btn svg {
  width: 1.3rem;
  height: 1.3rem;
}

/* Farbvarianten */
.icon-btn.primary {
  background: cornflowerblue;
  color: white;
  border-color: transparent;
}

.icon-btn.primary:hover {
  background: transparent;
  color: cornflowerblue;
  border-color: cornflowerblue;
}

.icon-btn.primary:focus:not(:hover) {
  background: cornflowerblue;
  color: white;
  border-color: transparent;
}

.icon-btn.danger:hover {
  color: #d9534f;
  border-color: #d9534f;
}

.icon-btn.warning:hover {
  color: #f0ad4e;
  border-color: #f0ad4e;
}

.icon-btn:focus:not(:hover) {
  color: var(--text-color);
  border-color: var(--border-color);
}

.icon-btn:focus {
  outline: none;
}

/* Mobile Touch-Ziel etwas größer */
@media (max-width: 600px) {
  .icon-btn {
    width: 2.8rem;
    height: 2.8rem;
  }

  .icon-btn svg {
    width: 1.5rem;
    height: 1.5rem;
  }
}
</style>
