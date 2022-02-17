#include <ArduinoJson.h>

#include "uplink_coap.h"

struct UplinkCoap uplinkCoap;
unsigned long lastMillis = 0;

// Callback function for messages from the server
void messageReceived(coapPacket &packet, IPAddress ip, int port)
{
  if (packet.code != CHANGED)
  {
    // Serial.println(payload);
    return;
  }

  char payload[packet.payloadlen + 1];
  memcpy(payload, packet.payload, packet.payloadlen);
  payload[packet.payloadlen] = 0;

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

  const char *cmd_type = doc["type"];
  const char *cmd_value = doc["value"];
  Serial.printf("Command Type: %s, Command Value: %s\n\r", cmd_type, cmd_value);

  // Your Code Here
}

void setup()
{
  Serial.begin(115200);
  // messageReceived is a callback function to handle data from server
  uplinkCoap.connect(messageReceived);
}

void loop()
{
  uplinkCoap.loop();

  // This example sends a dummy data to server every 30 seconds
  if (millis() - lastMillis > 1000 * 30 * 1)
  {
    lastMillis = millis();

    // Create Local Dummy Data
    float temperature = random(10, 60);
    char on_off[10];
    (random(2) == 0) ? strcpy(on_off, "on") : strcpy(on_off, "off");

    // Send data to server
    uplinkCoap.post(temperature, on_off);
  }
}