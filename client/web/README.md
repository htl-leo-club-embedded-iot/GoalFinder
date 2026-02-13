# Web Directory

This template should help get you started developing with Vue 3 in Vite.

## Recommended IDE Setup

[VSCode](https://code.visualstudio.com/) + [Volar](https://marketplace.visualstudio.com/items?itemName=Vue.volar) (and disable Vetur).

## Type Support for `.vue` Imports in TS

TypeScript cannot handle type information for `.vue` imports by default, so we replace the `tsc` CLI with `vue-tsc` for type checking. In editors, we need [Volar](https://marketplace.visualstudio.com/items?itemName=Vue.volar) to make the TypeScript language service aware of `.vue` types.

## Customize configuration

See [Vite Configuration Reference](https://vite.dev/config/).

## Project Setup

```sh
npm install
```

### Compile and Hot-Reload for Development

```sh
npm run dev
```

### Type-Check, Compile and Minify for Production

```sh
npm run build
```

## Structure

    .
    ├── .gitignore           # Git ignore rules for the web directory
    ├── env.d.ts             # TypeScript environment variable definitions
    ├── index.html           # Entry point for the web application
    ├── package.json         # npm configuration file for managing dependencies
    ├── README.md            # Documentation for the web directory
    ├── tsconfig.app.json    # TypeScript configuration for the application
    ├── tsconfig.json        # Base TypeScript configuration
    ├── tsconfig.node.json   # TypeScript configuration for Node.js scripts
    ├── vite.config.ts       # Vite configuration file for development and build
    ├── src/                 # Source code for the web application
    │   ├── App.vue          # Root Vue component
    │   ├── main.ts          # Entry point for the application
    │   ├── assets/          # Static assets such as styles and images
    │   │   ├── main.css     # Global CSS styles
    │   │   └── logos/       # Logo images
    │   ├── components/      # Reusable Vue components
    │   │   ├── BluetoothManager.vue
    │   │   ├── Button.vue
    │   │   ├── Card.vue
    │   │   ├── IconButton.vue
    │   │   ├── InputForm.vue
    │   │   ├── LaserSensorManager.vue
    │   │   ├── LedManager.vue
    │   │   ├── Modal.vue
    │   │   ├── NavigationBar.vue
    │   │   ├── Page.vue
    │   │   ├── SoftwareUpdateModal.vue
    │   │   ├── ToggleButton.vue
    │   │   ├── VibratiosensorManager.vue
    │   │   └── VolumeManager.vue
    │   │   └── icons/       # Icon components
    │   │       ├── AddIcon.vue
    │   │       ├── BluetoothIcon.vue
    │   │       ├── GearIcon.vue
    │   │       ├── InfoCircleIcon.vue
    │   │       ├── LightbulbIcon.vue
    │   │       ├── MinusIcon.vue
    │   │       ├── PlayIcon.vue
    │   │       ├── PowerIcon.vue
    │   │       ├── TrashIcon.vue
    │   │       ├── UpdateIcon.vue
    │   │       ├── VolumeUpIcon.vue
    │   │       └── WifiOffIcon.vue
    │   ├── models/          # TypeScript models for data structures
    │   │   ├── game.ts
    │   │   ├── player.ts
    │   │   └── team.ts
    │   ├── router/          # Vue Router configuration
    │   │   └── index.ts
    │   ├── stores/          # Pinia stores for state management
    │   │   ├── games.ts
    │   │   └── settings.ts
    │   └── views/           # Vue components for application views
    │       ├── AboutView.vue
    │       ├── HomeView.vue
    │       ├── NotFoundView.vue
    │       ├── Games/       # Game-related views
    │       │   ├── GamesView.vue
    │       │   ├── ShotChallengeView.vue
    │       │   └── TimedShotsChallengeView.vue
    │       └── Settings/    # Settings-related views
    │           ├── AudioSettingsView.vue
    │           ├── DevicesSettingsView.vue
    │           ├── SettingsView.vue
    │           └── SystemSettingsView.vue
