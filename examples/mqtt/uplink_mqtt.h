#ifndef UPLINK_MQTT_H
#define UPLINK_MQTT_H

#define PACKET_SIZE 256
#define MQTT_USER "test_bed_v1"
#define MQTT_PASS "xTxotwo2032sT3e"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>

#include <MQTT.h> // https://github.com/256dpi/arduino-mqtt
#include "uplink_mqtt_settings.h"

struct UplinkMQTT
{
  char pub_topic[100];
  char sub_topic[100];
  char key_serial[100];

  WiFiClientSecure net;
  MQTTClient client = MQTTClient(PACKET_SIZE);

  void connect(MQTTClientCallbackAdvanced messageReceived = NULL)
  {
    sprintf(key_serial, "%s%s", API_KEY, DEVICE_SERIAL);
    sprintf(pub_topic, "%s/%s/stat", BASE_TOPIC, key_serial);
    sprintf(sub_topic, "%s/%s/cmnd", BASE_TOPIC, key_serial);

    Serial.printf("pub: %s, sub: %s\n\r", pub_topic, sub_topic);

    WiFi.begin(WIFI_SSID, WIFI_PASS);
    net.setInsecure();
    Serial.print(F("checking wifi..."));
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(1000);
      Serial.print(".");
    }
    Serial.println(F("\nWiFi connected"));
    Serial.println(F("IP address: "));
    Serial.println(WiFi.localIP());

    // setup callback function
    client.onMessageAdvanced(messageReceived);

    client.begin(MQTT_SERVER, MQTT_SERVER_PORT, net);
    Serial.print(F("\nmosquitto connecting..."));
    while (!client.connect(key_serial, MQTT_USER, MQTT_PASS))
    {
      Serial.print(".");
      delay(1000);
    }
    Serial.print(F("\nconnected!"));
    client.subscribe(sub_topic);
  }

  bool connected()
  {
    return client.connected();
  }

  void loop()
  {
    delay(10);
    client.loop();
  }

  void messageSend(float temperature, const char *Power)
  {
    StaticJsonDocument<PACKET_SIZE> payloadJson;
    payloadJson["api_key"] = API_KEY;
    payloadJson["serial"] = DEVICE_SERIAL;
    payloadJson["data"]["temperature"] = temperature;
    payloadJson["data"]["Power"] = Power;

    char payload[PACKET_SIZE];
    serializeJson(payloadJson, payload);

    // message sent to server
    client.publish(pub_topic, payload);

    Serial.println(F("[Message Sent]"));
    Serial.println(payload);
  };
};
#endif
