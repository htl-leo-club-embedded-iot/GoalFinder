<script setup lang="ts">
import { computed, ref } from "vue"
import { RouterLink } from "vue-router"
import StarIcon from "@/components/icons/StarIcon.vue"
import { useFavouritesStore } from "@/stores/favourites"

const props = defineProps<{
  title: string
  description: string
  to: string
  gameName: string
  dragMode: boolean
}>()

const favouritesStore = useFavouritesStore()
const isFavourite = computed(() => favouritesStore.isFavourite(props.gameName))

let dragEnterCount = 0
const isDragOver = ref(false)
const isDragging = ref(false)

function toggleFavourite(): void {
  favouritesStore.toggleFavourite(props.gameName)
}

function onDragStart(event: DragEvent): void {
  isDragging.value = true
  if (event.dataTransfer) {
    event.dataTransfer.setData("text/plain", props.gameName)
    event.dataTransfer.effectAllowed = "move"
  }
}

function onDragOver(event: DragEvent): void {
  event.preventDefault()
  if (event.dataTransfer) {
    event.dataTransfer.dropEffect = "move"
  }
}

function onDragEnter(_event: DragEvent): void {
  dragEnterCount++
  isDragOver.value = true
}

function onDragLeave(_event: DragEvent): void {
  dragEnterCount--
  if (dragEnterCount === 0) {
    isDragOver.value = false
  }
}

function onDrop(event: DragEvent): void {
  event.preventDefault()
  isDragOver.value = false
  dragEnterCount = 0
  const draggedName = event.dataTransfer?.getData("text/plain")
  if (draggedName && draggedName !== props.gameName) {
    favouritesStore.moveBefore(draggedName, props.gameName)
  }
}

function onDragEnd(): void {
  isDragging.value = false
  isDragOver.value = false
  dragEnterCount = 0
}
</script>

<template>
  <RouterLink v-slot="{ navigate }" :to="to" custom>
    <div
      class="game-card"
      :class="{
        favourited: isFavourite,
        dragging: isDragging,
        'drag-over': isDragOver
      }"
      :draggable="dragMode"
      role="button"
      tabindex="0"
      @click="() => { if (!dragMode) navigate() }"
      @keydown.enter="() => { if (!dragMode) navigate() }"
      @dragstart="onDragStart"
      @dragover="onDragOver"
      @dragenter="onDragEnter"
      @dragleave="onDragLeave"
      @drop="onDrop"
      @dragend="onDragEnd"
    >
      <div class="star-wrapper" @click.stop="toggleFavourite">
        <div class="favourite-btn" :class="{ favourited: isFavourite }">
          <StarIcon :filled="isFavourite" />
        </div>
      </div>
      <div class="card-content">
        <h2>{{ $t(title) }}</h2>
        <p>{{ $t(description) }}</p>
      </div>
    </div>
  </RouterLink>
</template>

<style scoped>
.game-card {
  position: relative;
  width: 100%;
  max-width: 400px;
  padding: 1rem;
  border: 2px solid var(--border-color);
  border-radius: var(--corner-radius);
  background: var(--card-background-color);
  cursor: pointer;
  margin-top: 0.5rem;
  transition: border-color 0.2s, opacity 0.15s;
}

.game-card[draggable="true"] {
  cursor: grab;
}

.game-card[draggable="true"]:active {
  cursor: grabbing;
}

.game-card.dragging {
  opacity: 0.5;
}

.game-card.drag-over {
  opacity: 0.4;
}

.star-wrapper {
  position: absolute;
  top: 0.5rem;
  right: 0.5rem;
}

.favourite-btn {
  display: flex;
  align-items: center;
  justify-content: center;
  width: 28px;
  height: 28px;
  border: 2px solid var(--border-color);
  border-radius: var(--corner-radius);
  color: var(--border-color);
  transition: color 0.2s, border-color 0.2s;
}

.game-card.favourited {
  border-color: gold;
}

.favourite-btn.favourited {
  color: gold;
  border-color: gold;
}

.card-content {
  margin: 0.5rem;
  text-align: center;
}

.card-content h2 {
  margin: 0;
  text-align: center;
}

.card-content p {
  margin: 0.5rem 0 0 0;
  font-size: 0.95rem;
  line-height: 1.4;
}
</style>
