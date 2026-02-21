/*
 * ===============================================================================
 * (c) HTBLA Leonding 2024 - 2026
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Licensed under MIT License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the license.
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * All trademarks used in this document are property of their respective owners.
 * ===============================================================================
 */

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
                games: "Games",
                sessions: "Sessions",
                settings: "Settings",
                about: "About",
                game_card: "Create new game",
                settings_card: "Configure device",
                sessions_card: "Create new session"
            },
            games: {
                shot_challenge: "Throw-Challenge",
                shot_challenge_desc: "Test your accuracy! You have 60 seconds to score a goal. Stay in the rhythm, don't give in to the pressure and show who has the best throw!",
                timed_shots_challenge: "Timed-Game-Challenge",
                timed_shots_challenge_desc: "Every second counts. Score as many hits as possible in a fixed time limit. How many goals can score before the time runs out?",
                free_play: "Free Play",
                free_play_desc: "Just want to play without the hustle of setting up players? Don't look further, just start the \"Free Play\" game mode and go back to basics with just hit detection.",            },
            description: {
                settings_description: "Configure the device settings like sounds and devices.",
                sessions_description: "Add and remove persons into a basketball shot tracker",
                device_name_description: "Enter a new name for the device",
                ssid_password_description: "Set a WiFi password for the device",
                device_password_description: "Enter a new password for the device",
                dashboard_description: "Here you can create new games and configure the device settings.",
                gameModes_description: "Here you can choose a game mode",
                gameMode_description: "Select a game mode",
            },
            settings: {
                connection: "WiFi Connection",
                settings: "Settings",
                led: "LED",
                sensors: "Sensors",
                general: "General",
                wifi: "WiFi",
                devices: "Devices",
                audio: "Audio",
                system: "System",
                detection: "Detection",
                hit_detection: "Hit Detection",
                distance_only_hit_detection: "Distance-Only Hit Detection",
                distance_only_hit_detection_desc: "When enabled, only the laser distance sensor is used for hit detection. The vibration sensor will be ignored.",
                after_hit_timeout: "After-Hit Timeout",
                after_hit_timeout_desc: "Time in seconds the device waits after a hit before detecting the next one.",
                device_name: "Device name",
                device_password: "Device password",
                web_app: "Web App",
                accent_color: "Accent Color",
                theme: "Theme",
                theme_light: "Light",
                theme_dark: "Dark",
                theme_auto: "Auto",
                language: "Language",
                metronome_sound: "Metronome Sound",
                current_metronome_sound: "Current Metronome Sound: ",
                miss_sound: "Miss Sound",
                current_miss_sound: "Current Miss Sound: ",
                info: "Information",
                more_info: "For more information visit the ",
                doc: "GoalFinder User Documentation"
            },
            about: {
                about_desc: "This device was developed within the \"Smart Sport Assistance\" project in cooperation with the university of vienna by students of the HTBLA Leonding",
                developers: "Developed by:",
                visit_page: "Also visit our"
            },
            word : {
                on: "On",
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
                player: "Player",
                misses: "Misses",
                led_mode: "LED Mode",
                standard: "Standard",
                fade: "Fade",
                flash: "Flash",
                turbo: "Turbo",
                off:"Off",
                device_name: "Device Name",
                ssid_password: "WiFi Password",
                device_password: "Password",
                curr_mode: "Current Mode",
                curr_volume: "Current Volume",
                curr_sensitivity: "Current Sensitivity",
                mac_address: "Mac address",
                bluetooth_address: "Bluetooth address",
                software_update: "Software Update",
                curr_version: "Current Version",
                finish: "Finish",
                leaderboard: "Leaderboard",
                sound: "Sound",
                website: "website",
                brightness: "Brightness"
            },
            connection: {
                warning_title: "Connection Error",
                warning_message: "Could not connect to the GoalFinder. Please make sure you are connected to the device's WiFi GoalFinder."
            },
            auth: {
                title: "Authentication",
                description: "This GoalFinder is password protected. Please enter the password to continue.",
                login: "Login",
                password_placeholder: "Enter password",
                password_required: "Please enter a password.",
                invalid_password: "Invalid password. Please try again.",
                too_many_attempts: "Too many attempts. Please wait a minute and try again.",
                error: "Authentication failed. Could not reach the device."
            }
        },
        de : {
            header: {
                dashboard: "Goal Finder Dashboard",
                home: "Startseite",
                games: "Spiele",
                team_game: "Mannschaftsspiel",
                sessions: "Sitzungen",
                settings: "Einstellungen",
                about: "Über uns",
                game_card: "Neues Spiel erstellen",
                settings_card: "Einstellungen",
                sessions_card: "Neue Sitzung erstellen"
            },
            games: {
                shot_challenge: "Wurf-Challenge",
                shot_challenge_desc: "Teste deine Treffsicherheit! Du hast 60 Sekunden, um zu treffen. Bleib im Rhythmus, halte den Druck aus und zeig, wer den besten Wurf hat!",
                timed_shots_challenge: "Zeitspiel-Challenge",
                timed_shots_challenge_desc: "Hier zählt jede Sekunde! In einem festen Zeitlimit heißt es: So viele Treffer wie möglich landen. Wie viele Körbe schaffst du, bevor der Countdown endet?",
                free_play: "Freies Spiel",
                free_play_desc: "Möchten Sie einfach spielen, ohne den Aufwand, Spieler einzurichten? Dann suchen Sie nicht weiter: Starten Sie einfach den Spielmodus „Freies Spiel“ und kehren Sie zu den Grundlagen zurück – mit reiner Treffererkennung."
            },
            description: {
                settings_description: "Einstellungen für das Gerät vornehmen",
                gameMode_description: "Ein Spielmodus wählen",
                game_description: "Ein neues Spiel erstellen",
                device_name_description: "Einen neuen Namen für dieses Gerät eingeben.",
                ssid_password_description: "Ein WiFi-Passwort setzen.",
                device_password_description: "Ein neues Passwort für dieses Gerät eingeben.",
                dashboard_description: "Hier können Sie neue Spiele erstellen und die Geräteeinstellungen konfigurieren.",
                gameModes_description: "Hier können Sie einen Spielmodus auswählen"
            },
            settings: {
                connection: "WiFi Verbindung",
                settings: "Einstellungen",
                general: "Allgemein",
                wifi: "WiFi",
                devices: "Geräte",
                audio: "Audio",
                system: "System",
                detection: "Erkennung",
                hit_detection: "Trefferkennung",
                distance_only_hit_detection: "Nur Distanz-Trefferkennung",
                distance_only_hit_detection_desc: "Wenn aktiviert, wird nur der Laser-Distanzsensor zur Trefferkennung verwendet. Der Vibrationssensor wird ignoriert.",
                after_hit_timeout: "Nacherkennung-Timeout",
                after_hit_timeout_desc: "Zeit in Sekunden, die das Gerät nach einem Treffer wartet, bevor der nächste erkannt wird.",
                web_app: "Web App",
                accent_color: "Akzent Farbe",
                theme: "Erscheinungsbild",
                theme_light: "Hell",
                theme_dark: "Dunkel",
                theme_auto: "Automatisch",
                language: "Sprache",
                metronome_sound: "Metronome Ton",
                current_metronome_sound: "Aktueller Metronome Ton: ",
                miss_sound: "Fehlschuss Ton",
                current_miss_sound: "Aktueller Fehlschuss Ton: ",
                info: "Informationen",
                more_info: "Besuchen Sie für mehr Informationen die ",
                doc: "GoalFinder Benutzerdokumentation"
            },
            about: {
                about_desc: "Dieses Gerät wurde im Zuge des \"Smart Sport Assistance\" Projekt in Kooperation mit Universität Wien von Schülern der HTBLA Leonding entwickelt.",
                developers: "Entwickelt von:",
                visit_page: "Besuchen Sie auch unsere "
            },
            word : {
                on: "Ein",
                welcome_back: "Willkommen zurück",
                save: "Speichern",
                open: "Öffnen",
                volume: "Lautstärke",
                refresh: "Aktualisieren",
                sensitivity: "Empfindlichkeit",
                measuring: "Mess",
                distance: "Distanz",
                device_name: "Gerätename",
                ssid_password: "SSID Passwort",
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
                person_list: "Liste der Spieler",
                results: "Ergebnisse",
                player: "Spieler",
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
                bluetooth: "Bluetooth",
                sound: "Ton",
                website: "Webseite",
                brightness: "Helligkeit"
            },
            connection: {
                warning_title: "Verbindungsfehler",
                warning_message: "Es konnte keine Verbindung zum GoalFinder hergestellt werden. Bitte stellen Sie sicher, dass Sie mit dem WLAN des GoalFinders verbunden sind."
            },
            auth: {
                title: "Authentifizierung",
                description: "Dieser GoalFinder ist passwortgeschützt. Bitte geben Sie das Passwort ein, um fortzufahren.",
                login: "Anmelden",
                password_placeholder: "Passwort eingeben",
                password_required: "Bitte geben Sie ein Passwort ein.",
                invalid_password: "Ungültiges Passwort. Bitte versuchen Sie es erneut.",
                too_many_attempts: "Zu viele Versuche. Bitte warten Sie eine Minute und versuchen Sie es erneut.",
                error: "Authentifizierung fehlgeschlagen. Das Gerät konnte nicht erreicht werden."
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
