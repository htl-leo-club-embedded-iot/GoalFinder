import { defineStore } from "pinia"
import { ref } from "vue"

const STORAGE_KEY = "favourites"

export const useFavouritesStore = defineStore("favourites", () => {
  const favourites = ref<Set<string>>(loadFavourites())

  function loadFavourites(): Set<string> {
    let result = new Set<string>()
    try {
      const stored = localStorage.getItem(STORAGE_KEY)
      if (stored) {
        result = new Set(JSON.parse(stored) as string[])
      }
    } catch {
      // ignore parse errors
    }
    return result
  }

  function persist(): void {
    localStorage.setItem(STORAGE_KEY, JSON.stringify([...favourites.value]))
  }

  function isFavourite(name: string): boolean {
    let result = false
    if (favourites.value.has(name)) {
      result = true
    }
    return result
  }

  function toggleFavourite(name: string): void {
    if (favourites.value.has(name)) {
      favourites.value.delete(name)
    } else {
      favourites.value.add(name)
    }
    persist()
  }

  return {
    isFavourite,
    toggleFavourite,
  }
})
