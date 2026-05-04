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
                external_network_ssid_description: "Enter the SSID of the external network.",
                external_network_password_description: "Enter the password used by the external network.",
                enterprise_identity_description: "Set the identity used for enterprise authentication.",
                enterprise_username_description: "Set the username used for enterprise authentication.",
                enterprise_anonymous_identity_description: "Set the anonymous identity announced during enterprise setup.",
                enterprise_password_description: "Set the enterprise authentication password.",
                enterprise_ca_certificate_description: "Paste the CA certificate used by the enterprise network.",
                enterprise_client_certificate_description: "Paste the client certificate for enterprise authentication.",
                enterprise_client_private_key_description: "Paste the private key for enterprise authentication.",
                network_ip_address_description: "Enter the static IP address.",
                network_default_gateway_description: "Enter the default gateway address.",
                network_subnet_mask_description: "Enter the subnet mask.",
                network_dns_description: "Enter the DNS server address.",
                dashboard_description: "Here you can create new games and configure the device settings.",
                gameModes_description: "Here you can choose a game mode",
                gameMode_description: "Select a game mode",
            },
            settings: {
                connection: "WiFi Connection",
                connection_short: "Connection",
                connection_general: "Name & Security",
                settings: "Settings",
                led: "LED",
                sensors: "Sensors",
                general: "General",
                wifi: "WiFi",
                devices: "Devices",
                audio: "Audio",
                system: "System",
                logs: "Device Logs",
                logs_waiting: "Waiting for log messages...",
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
                miss_sound: "Miss Sound",
                hit_sound: "Hit Sound",
                waiting_sound: "Waiting Sound",
                info: "Information",
                more_info: "For more information visit the ",
                doc: "GoalFinder User Documentation",
                restart_title: "Restart Needed",
                restart_desc: "A device restart is necessary to apply this setting change.",
                restart_always: "Automatically restart device when this setting is changed and don't show again.",
                restart_now: "Restart Now",
                restart_later: "Restart Later",
                advanced: "Advanced Settings",
                advanced_desc: "This option gives you advanced and more granular settings option",
                dnsEnable: "Domain Name Service (DNS)",
                between_sound: "Time between metronome ticks",
                remove_password: "Remove Password",
                external_network: "External Network",
                use_external_network: "Use external network",
                external_network_ssid: "External network SSID",
                network_auth_mode: "Authentication mode",
                network_auth_open: "Open",
                network_auth_personal: "WPA/WPA2 Personal",
                network_auth_enterprise: "WPA/WPA2 Enterprise",
                external_network_password: "External network password",
                submit_secret: "Confirm password",
                enterprise_configuration: "Enterprise configuration",
                enterprise_identity: "Enterprise identity",
                enterprise_username: "Enterprise username",
                enterprise_anonymous_identity: "Enterprise anonymous identity",
                enterprise_password: "Enterprise password",
                enterprise_phase2_method: "Enterprise phase 2 method",
                enterprise_phase2_auto: "Automatic",
                enterprise_phase2_mschapv2: "MSCHAPv2",
                enterprise_phase2_gtc: "GTC",
                enterprise_ca_certificate: "CA certificate",
                enterprise_client_certificate: "Client certificate",
                enterprise_client_private_key: "Client private key",
                use_dhcp: "Use DHCP",
                manual_network_configuration: "Manual network configuration",
                network_ip_address: "IP address",
                network_default_gateway: "Default gateway",
                network_subnet_mask: "Subnet mask",
                network_dns: "DNS server",
                manual_network_advanced_hint: "Enable advanced settings to edit manual network address fields.",
                apply_network_configuration: "Apply network configuration",
                advancedEnterpriseSettings: "Advanced Enterprise Network Configuration Options"
            },
            about: {
                about_desc: "This device was developed within the \"Smart Sport Assistance\" project in cooperation with the university of Vienna by students of the HTBLA Leonding",
                developers: "Developed by:",
                visit_page: "Also visit our"
            },
            word : {
                on: "On",
                show: "Show",
                hide: "Hide",
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
                device_password: "Web App Password",
                curr_mode: "Current Mode",
                curr_volume: "Current Volume",
                curr_sensitivity: "Current Sensitivity",
                mac_address: "Mac address",
                bluetooth_address: "Bluetooth address",
                software_update: "Software Update",
                curr_version: "Current Version",
                web_app_version: "GoalFinder Version",
                finish: "Finish",
                leaderboard: "Leaderboard",
                sound: "Sound",
                website: "website",
                brightness: "Brightness",
                advanced: "Advanced",
                timing: "Timing",
                low: "Low",
                medium: "Medium",
                high: "High",
                max: "Max",
                small: "Small",
                middle: "Medium",
                normal: "Normal"
            },
            update: {
                upload: "Upload Update",
                uploading: "Updating... Please do not turn off the device.",
                success: "Update successful! The device is restarting.",
                failed: "Update failed. Please try again."
            },
            connection: {
                warning_title: "Connection Error",
                warning_message: "Could not connect to the GoalFinder. Please make sure you are connected to the device's WiFi GoalFinder.",
                ok: "OK",
                dont_show_again: "Don't show this again"
            },
            bin_warning: {
                title: "Update Warning",
                message: "You are about to upload a `.bin` file rather than a `.gfpkg` package. Please note that backporting to a version earlier than v0.4.0 is not recommended. Doing so will prevent updates through the Web App interface. Proceed only if you fully understand the potential risks and implications.",
                ok: "OK",
                dont_show_again: "Don't show this again"
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
                free_play_desc: "Möchten Sie einfach spielen, ohne den Aufwand, Spieler einzurichten? Freies spiel lässt sie einfach ohne Zwischenschritte spielen. Mit nur reiner Treffererkennung"
            },
            description: {
                settings_description: "Einstellungen für das Gerät vornehmen",
                gameMode_description: "Ein Spielmodus wählen",
                game_description: "Ein neues Spiel erstellen",
                device_name_description: "Einen neuen Namen für dieses Gerät eingeben.",
                ssid_password_description: "Ein WiFi-Passwort setzen.",
                device_password_description: "Ein neues Passwort für dieses Gerät eingeben.",
                external_network_ssid_description: "SSID des externen Netzwerks eingeben.",
                external_network_password_description: "Passwort des externen Netzwerks eingeben.",
                enterprise_identity_description: "Identität für die Enterprise-Authentifizierung eingeben.",
                enterprise_username_description: "Benutzernamen für die Enterprise-Authentifizierung eingeben.",
                enterprise_anonymous_identity_description: "Anonyme Identität für die Enterprise-Authentifizierung eingeben.",
                enterprise_password_description: "Passwort für die Enterprise-Authentifizierung eingeben.",
                enterprise_ca_certificate_description: "CA-Zertifikat für das Enterprise-Netzwerk einfügen.",
                enterprise_client_certificate_description: "Client-Zertifikat für die Enterprise-Authentifizierung einfügen.",
                enterprise_client_private_key_description: "Privaten Schlüssel für die Enterprise-Authentifizierung einfügen.",
                network_ip_address_description: "Statische IP-Adresse eingeben.",
                network_default_gateway_description: "Standard-Gateway eingeben.",
                network_subnet_mask_description: "Subnetzmaske eingeben.",
                network_dns_description: "DNS-Server-Adresse eingeben.",
                dashboard_description: "Hier können Sie neue Spiele erstellen und die Geräteeinstellungen konfigurieren.",
                gameModes_description: "Hier können Sie einen Spielmodus auswählen"
            },
            settings: {
                connection: "WiFi Verbindung",
                connection_short: "Verbindung",
                connection_general: "Name & Sicherheit",
                settings: "Einstellungen",
                general: "Allgemein",
                wifi: "WiFi",
                devices: "Geräte",
                audio: "Audio",
                system: "System",
                logs: "Geräte-Log",
                logs_waiting: "Warte auf Log-Nachrichten...",
                detection: "Erkennung",
                hit_detection: "Treffererkennung",
                distance_only_hit_detection: "Nur Distanz-Treffererkennung",
                distance_only_hit_detection_desc: "Wenn aktiviert, wird nur der Laser-Distanzsensor zur Treffererkennung verwendet. Der Vibrationssensor wird ignoriert.",
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
                miss_sound: "Fehlschuss Ton",
                hit_sound: "Treffer Ton",
                waiting_sound: "Warteton",
                info: "Informationen",
                more_info: "Besuchen Sie für mehr Informationen die ",
                doc: "GoalFinder Benutzerdokumentation",
                restart_title: "Neustart nötig",
                restart_desc: "Ein Neustart des Gerätes ist nötig um die Änderung zu übernehmen.",
                restart_always: "Automatisch das Gerät neustarten wenn diese Einstellung geändert wurde und nicht mehr anzeigen.",
                restart_now: "Jetzt Neustarten",
                restart_later: "Später Neustarten",
                advanced: "Erweiterte Einstellungen",
                advanced_desc: "Erweiterte Einstellungen ermöglichen Ihnen erweiterte und genauere Einstellungsmöglichkeiten.",
                dnsEnable: "Domain Name Service (DNS)",
                between_sound: "Zeitabstand zwischen Metronome Tönen",
                remove_password: "Passwort entfernen",
                external_network: "Externes Netzwerk",
                use_external_network: "Externes Netzwerk verwenden",
                external_network_ssid: "SSID des externen Netzwerks",
                network_auth_mode: "Authentifizierungsmodus",
                network_auth_open: "Offen",
                network_auth_personal: "WPA/WPA2 Personal",
                network_auth_enterprise: "WPA/WPA2 Enterprise",
                external_network_password: "Passwort des externen Netzwerks",
                submit_secret: "Passwort bestätigen",
                enterprise_configuration: "Enterprise-Konfiguration",
                enterprise_identity: "Enterprise-Identität",
                enterprise_username: "Enterprise-Benutzername",
                enterprise_anonymous_identity: "Enterprise-anonyme Identität",
                enterprise_password: "Enterprise-Passwort",
                enterprise_phase2_method: "Enterprise Phase-2-Methode",
                enterprise_phase2_auto: "Automatisch",
                enterprise_phase2_mschapv2: "MSCHAPv2",
                enterprise_phase2_gtc: "GTC",
                enterprise_ca_certificate: "CA-Zertifikat",
                enterprise_client_certificate: "Client-Zertifikat",
                enterprise_client_private_key: "Privater Client-Schlüssel",
                use_dhcp: "DHCP verwenden",
                manual_network_configuration: "Manuelle Netzwerkkonfiguration",
                network_ip_address: "IP-Adresse",
                network_default_gateway: "Standard-Gateway",
                network_subnet_mask: "Subnetzmaske",
                network_dns: "DNS-Server",
                manual_network_advanced_hint: "Aktivieren Sie die erweiterten Einstellungen, um manuelle Netzwerkadressfelder zu bearbeiten.",
                apply_network_configuration: "Netzwerkkonfiguration anwenden"
            },
            about: {
                about_desc: "Dieses Gerät wurde im Zuge des \"Smart Sport Assistance\" Projekt in Kooperation mit Universität Wien von Schülern der HTBLA Leonding entwickelt.",
                developers: "Entwickelt von:",
                visit_page: "Besuchen Sie auch unsere "
            },
            word : {
                on: "Ein",
                show: "Anzeigen",
                hide: "Verbergen",
                welcome_back: "Willkommen zurück",
                save: "Speichern",
                open: "Öffnen",
                volume: "Lautstärke",
                refresh: "Aktualisieren",
                sensitivity: "Empfindlichkeit",
                measuring: "Mess",
                distance: "Distanz",
                device_name: "Gerätename",
                ssid_password: "WiFi Passwort",
                device_password: "Web App Passwort",
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
                standard: "Standard",
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
                web_app_version: "GoalFinder Version",
                finish: "Beenden",
                leaderboard: "Rangliste",
                bluetooth: "Bluetooth",
                sound: "Ton",
                website: "Webseite",
                brightness: "Helligkeit",
                advanced: "Erweitert",
                timing: "Timing",
                low: "Niedrig",
                medium: "Mittel",
                high: "Hoch",
                max: "Maximum",
                small: "Klein",
                middle: "Mittel",
                normal: "Normal"
            },
            update: {
                upload: "Update hochladen",
                uploading: "Aktualisierung läuft... Bitte schalten Sie das Gerät nicht aus.",
                success: "Update erfolgreich! Das Gerät wird neu gestartet.",
                failed: "Update fehlgeschlagen. Bitte versuchen Sie es erneut."
            },
            connection: {
                warning_title: "Verbindungsfehler",
                warning_message: "Es konnte keine Verbindung zum GoalFinder hergestellt werden. Bitte stellen Sie sicher, dass Sie mit dem WLAN des GoalFinders verbunden sind.",
                ok: "OK",
                dont_show_again: "Nicht mehr anzeigen"
            },
            bin_warning: {
                title: "Update Warnung",
                message: "Sie sind gerade, eine .bin-Datei anstelle eines .gfpkg-Pakets hochzuladen. Bitte beachten Sie, dass ein Zurücksetzen auf eine Version älter als v0.4.0 nicht empfohlen wird. Dies wird dazu führen, dass das Gerät nicht mehr über die Web App aktualisierbar sein wird. Fahren Sie nur fort, wenn Sie die potenziellen Risiken verstehen.",
                ok: "OK",
                dont_show_again: "Nicht mehr anzeigen"
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
