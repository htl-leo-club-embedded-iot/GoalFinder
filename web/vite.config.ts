import { fileURLToPath, URL } from 'node:url'

import {build, defineConfig} from 'vite'
import vue from '@vitejs/plugin-vue'
import compression from "vite-plugin-compression2";

// https://vite.dev/config/
export default defineConfig({
  plugins: [
    vue(),
    compression(
        {
            algorithms: ['gzip'],
            threshold: 0,// ✅ force .gz extension
            deleteOriginalAssets: true,  // compress everything, even small files
        })
  ],
  resolve: {
    alias: {
      '@': fileURLToPath(new URL('./src', import.meta.url))
    }
  },
  build: {
    outDir: '../firmware/data/web',
    emptyOutDir: true,
    rollupOptions: {
      output: {
        assetFileNames: 'assets/[name][extname]',
        chunkFileNames: 'assets/[name].js',
        entryFileNames: 'assets/[name].js',
      }
    }
  }
})
