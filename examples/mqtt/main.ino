#include <ArduinoJson.h>

#include "uplink_mqtt.h"

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
