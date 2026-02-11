import { fileURLToPath, URL } from 'node:url'

import {build, defineConfig} from 'vite'
import vue from '@vitejs/plugin-vue'
import compression from "vite-plugin-compression2";

export default defineConfig({
  plugins: [
    vue(),
    compression(
        {
            algorithms: ['gzip'],
            threshold: 0,// force .gz extension
            deleteOriginalAssets: true,  // compress everything, even small files
        })
  ],
  resolve: {
    alias: {
      '@': fileURLToPath(new URL('./src', import.meta.url))
    }
  },
  build: {
    outDir: '../embedded/data/web',
    emptyOutDir: true,
    rollupOptions: {
      output: {
        // Keep filenames short — LittleFS has a 32-char filename limit add [name] for debugging to identify the file
        assetFileNames: 'assets/[hash][extname]',
        chunkFileNames: 'assets/[hash].js',
        entryFileNames: 'assets/[hash].js',
      }
    }
  }
})
