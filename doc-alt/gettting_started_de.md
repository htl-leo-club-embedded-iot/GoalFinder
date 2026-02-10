# Erste Schritte - Anleitung

## Inhaltsverzeichnis
- [Manuelles Flashen](#manuelles-flashen)
   - [Voraussetzungen (Tools)](#voraussetzungen-tools)
   - [Flashen des Systems](#flashen-des-systems)

- [Update über Webapp](#update-über-webapp)

Wenn der GoalFinder noch nicht aufgesetzt wurde oder der Flash-Speicher zurückgesetzt wurde, muss man die Firmware und das Filesystem manuell auf den GoalFinder flashen. Ansonsten kann man die neuste Firmware und das neuste Filesystem auf den GoalFinder über die Webapp laden.

## Manuelles Flashen

### Voraussetzungen (Tools)
- [CP210x Universal Windows Driver](https://www.silabs.com/developer-tools/usb-to-uart-bridge-vcp-drivers?tab=downloads)
- [Python](https://www.python.org/downloads/)

#### Installieren des Treibers
1. Entpacken Sie die Datei `CP210x_Universal_Windows_Driver.zip`
2. Öffnen Sie den entpackten Ordner
3. Drücken Sie mit der rechten Maustaste auf die Datei `silabser.inf` und wählen Sie `Installieren` aus
4. Folgen Sie den Anweisungen auf dem Bildschirm
5. Nun ist der erforderliche Treiber installiert

#### Installieren von Python und esptool.py
1. Installieren Sie die neuste Version von [Python](https://www.python.org/downloads/)
2. Führen Sie den Installer aus und folgen Sie den Anweisungen auf dem Bildschirm
3. Nun sollte Python installiert sein
4. Öffnen Sie die Eingabeaufforderung und führen Sie folgenden Befehl aus um `esptool.py` zu installieren.
   ``` 
   pip install esptool
   ```

### Flashen des Systems
1. Bauen Sie den GoalFinder auseinander um zum Micro-USB des ESP32 zu gelangen
2. Verbinden Sie den GoalFinder mithilfe eines Micro-USB Kabels mit dem Computer
3. Laden Sie die neusten `system.bin` und `littlefs.bin` Dateien von [Releases](https://github.com/2324-2bhif-pose-iot/GoalFinder/releases/latest) runter
4. Öffnen Sie die Eingabeaufforderung und navigieren Sie zum Ordner mit den heruntergeladenen Dateien mithilfe von dem Befehl `cd C:\Pfad\zum\Ordner`
5. Führen Sie folgenden Befehl in der Eingabeaufforderung aus um die Firmware zu flashen:
   ```
   python -m esptool -b 921600 write_flash 0x0 system.bin
   ```
6. Führen Sie folgenden Befehl in der Eingabeaufforderung aus um das Filesystem zu flashen:
   ```
   python -m esptool -b 921600 write_flash 2686976 littlefs.bin
   ```
7. Nun ist der GoalFinder betriebsbereit. Nun kann der GoalFinder durch die Webapp aktualisiert werden.

## Update über Webapp
1. Laden Sie die neusten `firmware.bin` und `littlefs.bin` Dateien von [Releases](https://github.com/2324-2bhif-pose-iot/GoalFinder/releases/latest) runter
2. Schließen Sie den GoalFinder an eine Stromquelle an
3. Verbinden Sie sich mit dem WiFi-Netz des GoalFinders
4. Öffnen Sie ihren Browser und geben Sie in die Adressleiste `goalfinder.local` ein
5. Es sollte sich nun die Webapp des GoalFinders öffnen
6. Navigieren Sie zu `Einstellungen > System`
7. Unter dem Punkt `Software Aktualisierung` drücken Sie auf `Auf Aktualisierung prüfen`
8. Wählen Sie zuerst `Firmware` und wählen Sie die zuvor heruntergeladene Datei `firmware.bin` aus
9. Warten Sie bis der Updatevorgang abgeschlossen wurde
10. Nun wird der Vorgang für das Filesystem wiederholt. Dabei wird `Filesystem` und anschließend die Datei `littlefs.bin` ausgewählt
11. Nun sollte der GoalFinder auf dem neusten Stand sein