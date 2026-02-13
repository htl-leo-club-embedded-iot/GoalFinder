# Goal Finder

IoT project in scope of POSE-PRPR and WLC, HTL Leonding, 2BHIF, JG 2324 in cooperation with Universität Wien.

[Erste Schritte](docs/gettting_started_de.md)

## Introduction

When visually impaired people play basket ball, they need acoustic assistance for locating the basket and as feedback if the basket is hit.

For locating the basket, a constant beeping is sufficient. For a hit, a triumph fanfare may be played. Miss detection may result in an according tune.

![Basketball](./docs/images/basket_ball_players.jpg)

Such assistance device may be directly mounted on the basket rack. They shall be easily mountable, e.g. by sliding them on the board or basket.

For hit detection, a short range laser distance sensor (ToF - time of flight sensor) is used. It is looking through the mesh of the basket and is therefore capable detecting a ball falling through the basket.

![Sensor Sketch](./docs/images/sensor_measurements_sketch.png)

Optionally, hitting the basket-board may be detected using an acceleration sensor. When a ball hits the board or the ring, heavy motion can be expected and detected. If the ball does not pass the basket in time, a miss is detected.

The sensor device shall be controlled via a web application designed for mobile devices, to operate the system on the (basketball) field. The application may also assist in the game, for instance by automatically counting hits for multiple players.

## Project Goals

A basketball assistance device as described about shall be developed and deployed in the field.

## Hardware Platform

+ SoC: ESP32, see [Random Nerd Tutorials: ESP32- Pinout](https://randomnerdtutorials.com/esp32-pinout-reference-gpios/)
+ Distance Sensor: ToF sensor connected via I²C, such as VL53L0X. see [YouTube: ToF used with ESP32](https://www.youtube.com/watch?v=HeMlcCDYCEk).
+ Option: Acceleration sensor (TBD).
+ Audio: ESP32 DAC with external pre-amplifier, see [YouTube: ESP32 Audio: I2S & Built-In DACs Explained](https://www.youtube.com/watch?v=lgDu88Y411o)
+ Power supply: Grid via power adapter or power bank.

## Use Cases and User Stories

1. As a user, I want to control the device via web application on a smart phone as well as on a desktop PC.
1. As a player, I want the device to provide acoustic hints of the basket location.
1. As a player, I want the device to sound a triumph tune when the ball hits the basket.
1. As a player, it would be great to hear a sound when the ball hits the basket rack but not the basket.
1. As a user, I want to connect to the device via WiFi network that is spread by the device.
1. As a user, I want to integrate the device into my WiFi network easily.
1. As a user, I want that the device time is synchronized with the real time of my timezone.
1. As a user, I want to update the device software over the air.

## Tasks

This section describes the first task to be tackled on a high-level base.

1. Specify a roadmap / time table for the given tasks and user story implementation.
1. Develop a software design showing the major software components and their relations as well as required communication protocols.
1. Integrate a WiFi setup and configuration on the device.
1. Integrate a web server on the device.
1. Integrate an SNTP client on the device, including SNTP configuration via integrated web server.
1. Implement the SW-component to measure distances via distance senor.
1. Option: Implement SW-component to measure motion via motion sensor.
1. Implement hit / miss detection based on the measured values.
1. Implement audio output
1. Integrate suitable audio tracks for signalling events.
1. Option: Implement audio feedback via Bluetooth.
1. Implement a web front-end suitable for configuring the sensor device
1. Implement a web front-end for game play.
1. Integrate and enable SWUOTA (software update over the air).
