#ifndef UPLINK_COAP_H
#define UPLINK_COAP_H

#define PACKET_SIZE 256

#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>

#include "coap_client.h"
#include "uplink_coap_settings.h"

struct UplinkCoap
{
  char resourcePath[100];

  CoapClient coap;

  void connect(callback messageReceived = NULL)
  {
    sprintf(resourcePath, "%s/%s%s", BASE_RESOURCE_PATH, API_KEY, DEVICE_SERIAL);

    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.print(F("checking wifi..."));
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }
    Serial.println(F("\nWiFi connected"));
    Serial.println(F("IP address: "));
    Serial.println(WiFi.localIP());

    // setup callback function
    coap.response(messageReceived);

    // start coap client
    coap.start(COAP_SERVER, COAP_SERVER_PORT, resourcePath);
    coap.subscribe();
  }

  void loop()
  {
    delay(1000);
    coap.loop();
  }

  void post(float temperature, const char *Power)
  {
    StaticJsonDocument<PACKET_SIZE> payloadJson;
    payloadJson["api_key"] = API_KEY;
    payloadJson["serial"] = DEVICE_SERIAL;
    payloadJson["data"]["temperature"] = temperature;
    payloadJson["data"]["Power"] = Power;

    char payload[PACKET_SIZE];
    serializeJson(payloadJson, payload);

    // send observtion request
    coap.observe();
    // message sent to server
    coap.post(payload);

    Serial.println(F("[Message Sent]"));
    Serial.println(payload);
  }
};
#endif
