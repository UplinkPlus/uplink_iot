# Uplink+ IOT([CoAP](https://ko.wikipedia.org/wiki/CoAP))

<strong>Arduino</strong> 프레임워크와 <strong>CoAP</strong> 프로토콜을 사용하여 디바이스 장치(센서, 액츄에이터, 스위치 등)를 <strong>업링크+</strong> 서버와 데이터를 주고 받을 수 있도록 연결하는 방법입니다.

## 필요한것들

1. MCU Device: ESP32
2. IDE: Arduino IDE (or VSCode+PlatformIO Home)
3. 라이브러리: [ArduinoJson](https://arduinojson.org/v6/doc/)

## 라이브러리 설치방법
- 검색 키워드: ArduinoJson
- [Installing Libraries in Arduino.cc IDE](https://docs.arduino.cc/software/ide-v1/tutorials/installing-libraries):
- [Installing Libraries in PlatformIO](https://docs.platformio.org/en/latest/librarymanager/index.html):

## 시작하기
### Step1: [https://iot.uplink.plus/](https://iot.uplink.plus/)에서 연결하고자 하시는 디바이스를 등록해 주세요.

### Step2: 디바이스 추가 마지막 페이지에서 해당 다바이스를 위한 업링크+ 라이브러리 파일들을 다운받으시거나, 이미 디바이스를 등록하셨다면 해당 디바이스 설정에 가셔서 업링크 라이브러리 파일들을 다운받으세요.

### Step3: 다운받으신 파일중에 밑에 파일 값들을 본인의 와이파이 환경에 맞추어 수정해주세요.
#### uplink_coap_settings.h
- WIFI_SSID, WIFI_PASS 값들을 입력해주세요.

### Step4: 밑에 예시를 참고하시어 main.ino을 생성해주세요.
#### main.ino
- 서버에서 데이터를 받는 기능이 필요하시다면 messageReceived() 함수를 수정해주세요. 이 함수는 서버에서 오는 데이터를 다루는 콜백 함수입니다.

## main.ino 예시
```c++
#include <ArduinoJson.h>

#include "uplink_coap.h"

struct UplinkCoap uplinkCoap;
unsigned long lastMillis = 0;

// Callback function for messages from the server
void messageReceived(coapPacket &packet, IPAddress ip, int port)
{
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

  const char *cmd_type = doc["command_type"];
  const char *cmd_value = doc["command_value"];
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
```

## Copyright

Copyright © 2022 uplink All rights reserved.
