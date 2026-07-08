import { defineStore } from "pinia"
import { ref, computed } from "vue"

const FAV_KEY = "favourites"
const ORDER_KEY = "gameOrder"
const DEFAULT_ORDER = ["shot-challenge", "timed-shots-challenge", "free-play"]

export const useFavouritesStore = defineStore("favourites", () => {
  const favourites = ref<Set<string>>(loadSet(FAV_KEY))
  const gameOrder = ref<string[]>(loadOrder())

  function loadSet(key: string): Set<string> {
    let result = new Set<string>()
    try {
      const stored = localStorage.getItem(key)
      if (stored) {
        result = new Set(JSON.parse(stored) as string[])
      }
    } catch {
      // ignore parse errors
    }
    return result
  }

  function loadOrder(): string[] {
    let result = DEFAULT_ORDER
    try {
      const stored = localStorage.getItem(ORDER_KEY)
      if (stored) {
        result = JSON.parse(stored) as string[]
      }
    } catch {
      // ignore parse errors
    }
    return result
  }

  function persist(): void {
    localStorage.setItem(FAV_KEY, JSON.stringify([...favourites.value]))
    localStorage.setItem(ORDER_KEY, JSON.stringify(gameOrder.value))
  }

  const sortedGameNames = computed(() => {
    const fav: string[] = []
    const nonFav: string[] = []
    for (const name of gameOrder.value) {
      if (favourites.value.has(name)) {
        fav.push(name)
      } else {
        nonFav.push(name)
      }
    }
    return [...fav, ...nonFav]
  })

  function isFavourite(name: string): boolean {
    let result = false
    if (favourites.value.has(name)) {
      result = true
    }
    return result
  }

  function toggleFavourite(name: string): void {
    const wasFav = favourites.value.has(name)
    const idx = gameOrder.value.indexOf(name)
    gameOrder.value.splice(idx, 1)
    if (wasFav) {
      favourites.value.delete(name)
      const favCount = gameOrder.value.filter((g) => favourites.value.has(g)).length
      gameOrder.value.splice(favCount, 0, name)
    } else {
      favourites.value.add(name)
      gameOrder.value.unshift(name)
    }
    persist()
  }

  function moveBefore(name: string, targetName: string): void {
    const fromIdx = gameOrder.value.indexOf(name)
    if (fromIdx !== -1) {
      gameOrder.value.splice(fromIdx, 1)
      const toIdx = gameOrder.value.indexOf(targetName)
      if (toIdx !== -1) {
        gameOrder.value.splice(toIdx, 0, name)
        persist()
      } else {
        gameOrder.value.splice(fromIdx, 0, name)
      }
    }
  }

  return {
    sortedGameNames,
    isFavourite,
    toggleFavourite,
    moveBefore,
  }
})
