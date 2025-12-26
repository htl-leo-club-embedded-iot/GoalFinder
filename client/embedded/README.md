# Embedded Directory

## Structure

    .
    ├── .gitignore           Git ignore rules for the client directory
    ├── merge-bin.py         Python script for merging binary files
    ├── platformio.ini       PlatformIO configuration file
    ├── data/                Data files for the firmware
    │   └── web/             Web assets for the firmware
    │       └── assets/      CSS files for web views
    │           ├── GamesView.css
    │           ├── GeneralSettingsView.css
    │           └── NotFoundView.css
    ├── lib/                Libraries used in the firmware
    │   ├── file_system/    File system management library
    │   │   ├── FileSystem.cpp
    │   │   └── FileSystem.h
    │   ├── lib_audioplayer/    Audio player library
    │   │   ├── library.json
    │   │   ├── include/
    │   │   │   └── AudioPlayer.h
    │   │   └── src/
    │   │       └── AudioPlayer.cpp
    │   ├── lib_bluetoothmanager/   Bluetooth manager library
    │   │   ├── library.json
    │   │   ├── include/
    │   │   │   └── BluetoothManager.h
    │   │   └── src/
    │   │       └── BluetoothManager.cpp
    │   ├── lib_settings/    # Settings management library
    │   │   ├── hal_selector.py
    │   │   ├── library.json
    │   │   ├── include/
    │   │   │   └── system/
    │   │   │       └── Settings.h
    │   │   └── src/
    │   │       └── hal/
    │   │           └── system/
    │   │               ├── dummy/
    │   │               │   └── DummySettings.cpp
    │   │               ├── esp32/
    │   │               │   └── Esp32Settings.cpp
    │   │               └── esp8266/
    │   │                   └── Esp8266Settings.cpp
    │   ├── lib_tofsensor/   Time-of-flight sensor library
    │   │   ├── library.json
    │   │   ├── include/
    │   │   │   └── ToFSensor.h
    │   │   └── src/
    │   │       └── ToFSensor.cpp
    │   └── lib_vibrationsensor/    Vibration sensor library
    │       ├── library.json
    │       ├── include/
    │       │   └── VibrationSensor.h
    │       └── src/
    │           └── VibrationSensor.cpp
    ├── src/    Main application source code
    │   ├── GoalfinderApp.cpp
    │   ├── GoalfinderApp.h
    │   ├── LedController.cpp
    │   ├── LedController.h
    │   ├── LedMode.h
    │   ├── main.cpp    Entry point of the firmware
    │   ├── Settings.cpp
    │   ├── Settings.h
    │   ├── Singleton.h
    │   └── web/             # Web-related source code
    │       ├── SNTP.cpp
    │       ├── SNTP.h
    │       ├── SoftwareUpdater.cpp
    │       ├── SoftwareUpdater.h
    │       ├── WebServer.cpp
    │       ├── WebServer.h
    │       ├── WifiManager.cpp
    │       └── WifiManager.h