# UPLINK IOT([CoAP](https://ko.wikipedia.org/wiki/CoAP))

This example shows how a sensor can be connected and communicate with Uplink server using CoAP protocol.


## Requirements

1. Device: ESP32
2. IDE: Arduino IDE (or VSCode+PlatformIO Home)

## Library
1. [ArduinoJson](https://arduinojson.org/v6/doc/)
### Installing Libraries in the [Arduino.cc IDE](https://docs.arduino.cc/software/ide-v1/tutorials/installing-libraries):
Search Keyword: ArduinoJson
### Installing Libraries in [PlatformIO](https://docs.platformio.org/en/latest/librarymanager/index.html):
Search Keyword: ArduinoJson

## Getting Started
* if you generated these file on the https://iot.uplink.plus/, some fields are already changed as your device.
### uplnik_coap_settings.h
1. Edit WIFI_SSID, WIFI_PASS values
2. Edit API_KEY, DEVICE_SERIAL values
### uplink_coap.h
1. Edit post() and callback_response() functions

## Copyright

Copyright © 2022 uplink All rights reserved.