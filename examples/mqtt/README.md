# UPLINK IOT([MQTT](https://en.wikipedia.org/wiki/MQTT))

This example shows how a sensor can be connected and communicate with Uplink server using MQTT protocol.


## Requirements

1. Device: ESP32
2. IDE: Arduino IDE (or VSCode+PlatformIO Home)

## Library
1. [ArduinoJson](https://arduinojson.org/v6/doc/)
2. [256dpi/MQTT by Joerg Wende](https://github.com/256dpi/arduino-mqtt?utm_source=platformio&utm_medium=piohome)
### Installing Libraries in the [Arduino.cc IDE](https://docs.arduino.cc/software/ide-v1/tutorials/installing-libraries):
Search Keyword: ArduinoJson, MQTT lwmqtt
### Installing Libraries in [PlatformIO](https://docs.platformio.org/en/latest/librarymanager/index.html):
Search Keyword: ArduinoJson, MQTT lwmqtt

## Getting Started
* if you generated these file on the https://iot.uplink.plus/, some fields are already changed as your device.
### uplnik_mqtt_settings.h
1. Edit WIFI_SSID, WIFI_PASS values
2. Edit API_KEY, DEVICE_SERIAL values
### uplink_mqtt.h
1. Edit messageSend() and messageReceived() functions

## Example
example of main.ino

## Copyright

Copyright © 2022 uplink All rights reserved.