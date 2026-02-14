import './assets/main.css'

import { createApp } from 'vue'
import { createPinia } from 'pinia'

import App from './App.vue'
import router from './router'
import {createI18n} from "vue-i18n";

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
                connection: "WiFi connection",
                settings: "Settings",
                led: "LED",
                sensoren: "Sensoren",
                general: "General",
                wifi: "WiFi",
                devices: "Devices",
                audio: "Audio",
                system: "System",
                device_name: "Device name",
                device_password: "Device password",
                web_app: "Web App",
                accent_color: "Accent Color",
                theme: "Theme",
                theme_light: "Light",
                theme_dark: "Dark",
                theme_auto: "Auto",
                language: "Language"
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
                search_for_update: "Upload Firmware",
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
                connection: "WiFi Verbindung",
                settings: "Einstellungen",
                general: "Allgemein",
                wifi: "WLAN",
                devices: "Geräte",
                audio: "Audio",
                system: "System",
                web_app: "Web App",
                accent_color: "Akzent Farbe",
                theme: "Erscheinungsbild",
                theme_light: "Hell",
                theme_dark: "Dunkel",
                theme_auto: "Automatisch",
                language: "Sprache"
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
                search_for_update: "Firmware hochladen",
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

// Restore saved accent color
const savedAccentColor = localStorage.getItem('accent-color');
if (savedAccentColor) {
    document.documentElement.style.setProperty('--accent-color', savedAccentColor);
}

// Restore saved theme (light / dark / auto)
const savedTheme = localStorage.getItem('theme');
if (savedTheme === 'light' || savedTheme === 'dark') {
    document.documentElement.setAttribute('data-theme', savedTheme);
} else {
    document.documentElement.removeAttribute('data-theme');
}

// Restore saved language
const savedLang = localStorage.getItem('language');
if (savedLang && (savedLang === 'en' || savedLang === 'de')) {
    i18n.global.locale = savedLang;
}

app.mount('#app')
