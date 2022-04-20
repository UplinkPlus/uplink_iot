# Uplink+ IOT([MQTT](https://en.wikipedia.org/wiki/MQTT))

This example shows how a device(sensor, actuator, switch, etc.) can be connected and communicate with <strong>Uplink+</strong> server using <strong>Arduino</strong> framework and <strong>MQTT</strong> protocol.


## Requirements

1. MCU Device: ESP32
2. IDE: Arduino IDE (or VSCode+PlatformIO Home)
3. Library: [ArduinoJson](https://arduinojson.org/v6/doc/), [MQTT by Joel Gaehwiler](https://github.com/256dpi/arduino-mqtt)

## How to install Library?
- Search Keyword: ArduinoJson, MQTT
- [Installing Libraries in Arduino.cc IDE](https://docs.arduino.cc/software/ide-v1/tutorials/installing-libraries):
- [Installing Libraries in PlatformIO](https://docs.platformio.org/en/latest/librarymanager/index.html):

## Getting Started
### Step1: Add the device which you want to connect with Uplink+ server on [https://iot.uplink.plus/](https://iot.uplink.plus/).

### Step2: You can download uplink+ libraries on the last page of adding device page or if you already added the device,  go to the edit page of the device and download Uplink+ libraries for your device.

### Step3: Edit below files
#### uplink_mqtt_settings.h
- Edit WIFI_SSID, WIFI_PASS values

### Step4: Create your main.ino
#### main.ino
- Edit messageReceived() function which is a callback function to handle data from server 

## main.ino Example
```c++
#include <ArduinoJson.h>

#include "uplink_mqtt.h"
#include "uplink_mqtt_settings.h"

UplinkMQTT uplinkMQTT;
unsigned long lastMillis = 0;

void messageReceived(MQTTClient *client, char *topic, char *payload, int length)
{
  Serial.println(F("[Message Recieved]"));
  StaticJsonDocument<256> doc;
  DeserializationError err = deserializeJson(doc, payload);
  if (err)
  {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(err.f_str());
    Serial.print(F("The failed payload: "));
    Serial.println(payload);
    return;
  }

  const char *cmd_type = doc["command_type"];
  const char *cmd_value = doc["command_value"];
  Serial.printf("Command Type: %s, Command Value: %s\n\r", cmd_type, cmd_value);

  // Your Code Here
}

void setup()
{
  Serial.begin(115200);
  uplinkMQTT.connect(messageReceived);
}

void loop()
{
  uplinkMQTT.loop();
  if (!uplinkMQTT.connected())
  {
    // messageReceived is a callback function to handle data from server
    uplinkMQTT.connect(messageReceived);
  }

  // This example sends a dummy data to server every 30 seconds
  if (millis() - lastMillis > 1000 * 30 * 1)
  {
    lastMillis = millis();

    // Create Local Dummy Data
    float temperature = random(10, 60);
    char on_off[10];
    (random(2) == 0) ? strcpy(on_off, "on") : strcpy(on_off, "off");

    // Send data to server
    uplinkMQTT.messageSend(temperature, on_off);
  }
}
```

## Copyright

Copyright © 2022 uplink All rights reserved.
