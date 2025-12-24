import './assets/main.css'

import { createApp } from 'vue'
import { createPinia } from 'pinia'

import App from './App.vue'
import router from './router'
import {createI18n} from "vue-i18n";
import Modal from "@/components/Modal.vue";

const app = createApp(App)

const i18n = createI18n({
    locale: "de",
    fallbackLocale: "en",
    messages: {
        en: {
            header: {
                dashboard: "Goal Finder Dashboard",
                home: "Home",
                game: "Game",
                sessions: "Sessions",
                settings: "Settings",
                about: "About",
                game_card: "Create new game",
                settings_card: "Configure device",
                sessions_card: "Create new session"
            },
            description: {
                settings_description: "Configure the device settings like sounds and devices.",
                sessions_description: "Add and remove persons into a basketball shot tracker",
                device_name_description: "Enter a name for the device.",
                device_password_description: "Enter a password for the device.",
                dashboard_description: "Here you can create new games and configure the device settings."
            },
            settings: {
                settings: "Settings",
                led: "LED",
                sensoren: "Sensoren",
                general: "General",
                wifi: "WiFi",
                devices: "Devices",
                audio: "Audio",
                system: "System",
                device_name: "Device name",
                device_password: "Device password"
            },
            word : {
                on: "Ein",
                welcome_back: "Welcome back",
                save: "Save",
                open: "Open",
                volume: "Volume",
                refresh: "Refresh",
                sensitivity: "Sensitivity",
                measuring: "Measuring",
                distance: "Distance",
                restart: "Restart",
                factory_settings_reset: "Reset to factory settings",
                search_for_update: "Search for update",
                light: "Light",
                dark: "Dark",
                add_person: "Add Person",
                basketball_shot_tracker: "Basketball Shot Tracker",
                hit: "Hit",
                miss: "Miss",
                remove: "Remove",
                person_list: "Person List",
                results: "Results",
                hits: "Hits",
                misses: "Misses",
                led_mode: "LED Mode",
                standard: "Standard",
                fade: "Fade",
                flash: "Flash",
                turbo: "Turbo",
                off:"Off",
                curr_mode: "Current Mode",
                curr_volume: "Current Volume",
                curr_sensitivity: "Current Sensitivity",
                mac_address: "Mac address",
                bluetooth_address: "Bluetooth address",
                software_update: "Software Update",
                curr_version: "Current Version",
                finish: "Finish",
                leaderboard: "Leaderboard"
            }
        },
        de : {
            header: {
                dashboard: "Goal Finder Dashboard",
                home: "Startseite",
                games: "Spiele",
                game_shot_challenge: "Wurf-Challenge",
                game_timed_shots_challenge: "Zeitspiel-Challenge",
                team_game: "Mannschaftsspiel",
                sessions: "Sitzungen",
                settings: "Einstellungen",
                about: "Über uns",
                game_card: "Neues Spiel erstellen",
                settings_card: "Einstellungen",
                sessions_card: "Neue Sitzung erstellen"
            },
            description: {
                settings_description: "Einstellungen für das Gerät vornehmen",
                gameMode_description: "Ein Spielmodus wählen",
                game_description: "Ein neues Spiel erstellen",
                device_name_description: "Einen neuen Namen für dieses Gerät eingeben.",
                device_password_description: "Ein neues Passwort für dieses Gerät eingeben.",
                dashboard_description: "Hier können Sie neue Spiele erstellen und die Geräteeinstellungen konfigurieren.",
                gameModes_description: "Hier können Sie einen Spielmodus auswählen"
            },
            settings: {
                settings: "Einstellungen",
                general: "Allgemein",
                wifi: "WLAN",
                devices: "Geräte",
                audio: "Audio",
                system: "System"
            },
            word : {
                welcome_back: "Willkommen zurück",
                save: "Speichern",
                open: "Öffnen",
                volume: "Lautstärke",
                refresh: "Aktualisieren",
                sensitivity: "Empfindlichkeit",
                measuring: "Mess",
                distance: "Distanz",
                device_name: "Gerätename",
                device_password: "Passwort",
                restart: "Neustarten",
                factory_settings_reset: "Auf Werkseinstellungen zurücksetzen",
                search_for_update: "Auf Aktualisierungen prüfen",
                light: "Hell",
                dark: "Dunkel",
                add_person: "Person Hinzufügen",
                basketball_shot_tracker: "Basketball Schusszähler",
                hit: "Treffer",
                miss: "Fehlschuss",
                remove: "Entfernen",
                person_list: "Liste der Personen",
                results: "Ergebnisse",
                hits: "Treffer",
                misses: "Fehlschüsse",
                led_mode:"LED Modus",
                standard: "Standart",
                flash: "Blitzartig",
                turbo: "Turbo",
                off: "Aus",
                curr_mode: "Ausgewählter Modus",
                curr_volume: "Derzeitige Lautstärke",
                curr_sensitivity: "Derzeitige Empfindlichkeit",
                mac_address: "Mac Adresse",
                bluetooth_address: "Bluetooth Adresse",
                software_update: "Software Aktualisierung",
                curr_version: "Derzeitige Version",
                finish: "Beenden",
                leaderboard: "Rangliste",
                bluetooth: "Bluetooth"
            }
        }
    }
});

app.use(createPinia());
app.use(router);
app.use(i18n);

app.mount('#app')
