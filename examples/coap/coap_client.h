#ifndef UPLINK_COAP_CLIENT_H
#define UPLINK_COAP_CLIENT_H

#include <WiFiUdp.h>

#define OBSERVE_REGISTER 0
#define OBSERVE_DEREGISTER 1

// current coap attributes
#define COAP_DEFAULT_PORT 5683
#define COAP_HEADER_SIZE 4
#define COAP_VERSION 1

// configuration
#define MAX_OPTION_NUM 10
#define BUF_MAX_SIZE 250

#define COAP_OPTION_DELTA(v, n) (v < 13 ? (*n = (0xFF & v)) : (v <= 0xFF + 13 ? (*n = 13) : (*n = 14)))

// coap message types
typedef enum
{
  COAP_CON = 0,
  COAP_NONCON = 1,
  COAP_ACK = 2,
  COAP_RESET = 3
} COAP_TYPE;

// coap method values
typedef enum
{
  COAP_EMPTY = 0,
  COAP_GET = 1,
  COAP_POST = 2,
  COAP_PUT = 3,
  COAP_DELETE = 4
} COAP_METHOD;

// coap option values
typedef enum
{
  COAP_IF_MATCH = 1,
  COAP_URI_HOST = 3,
  COAP_E_TAG = 4,
  COAP_IF_NONE_MATCH = 5,
  COAP_OBSERVE = 6,
  COAP_URI_PORT = 7,
  COAP_LOCATION_PATH = 8,
  COAP_URI_PATH = 11,
  COAP_CONTENT_FORMAT = 12,
  COAP_MAX_AGE = 14,
  COAP_URI_QUERY = 15,
  COAP_ACCEPT = 17,
  COAP_LOCATION_QUERY = 20,
  COAP_BLOCK_2 = 23,
  COAP_PROXY_URI = 35,
  COAP_PROXY_SCHEME = 39
} COAP_OPTION_NUMBER;

// coap code values
typedef enum
{
  CREATED = 65,
  DELETED = 66,
  VALID = 67,
  CHANGED = 68,
  CONTENT = 69,
  BAD_REQUEST = 128,
  UNAUTHORIZED = 129,
  BAD_OPTION = 130,
  FORBIDDEN = 131,
  NOT_FOUND = 132,
  METHOD_NOT_ALLOWED = 133,
  NOT_ACCEPTABLE = 134,
  PRECONDITION_FAILED = 140,
  REQUEST_ENTITY_TOO_LARGE = 141,
  UNSUPPORTED_CONTENT_FORMAT = 143,
  INTERNAL_SERVER_ERROR = 160,
  NOT_IMPLEMENTED = 161,
  BAD_GATEWAY = 162,
  SERVICE_UNAVAILABLE = 163,
  GATEWAY_TIMEOUT = 164,
  PROXYING_NOT_SUPPORTED = 165
} COAP_CODE_NUMBER;

// coap option class
class coapOption
{
public:
  uint8_t number;
  uint8_t length;
  uint8_t *buffer;
};

// coap packet class
class coapPacket
{
public:
  uint8_t type;
  uint8_t code;
  uint8_t *token;
  uint8_t tokenlen;
  uint8_t *payload;
  uint8_t payloadlen;
  uint16_t messageid;
  uint8_t optionnum;
  coapOption options[MAX_OPTION_NUM];
};

typedef void (*callback)(coapPacket &, IPAddress, int);

struct CoapClient
{
  IPAddress ip;
  const char *hostName = nullptr;
  int port;
  const char *url = nullptr;

  WiFiUDP udp;
  callback resp;

  // coap client begin
  bool start(IPAddress ip, int port, const char url[])
  {
    this->ip = ip;
    this->port = port;
    this->url = strdup(url);
    udp.begin(port);
    return true;
  }

  // coap client begin
  bool start(const char hostName[], int port, const char url[])
  {
    // set hostname and port
    this->hostName = strdup(hostName);
    this->port = port;
    this->url = strdup(url);
    udp.begin(port);
    return true;
  }

  // get request
  uint16_t get()
  {
    return send(COAP_CON, COAP_GET, "");
  }

  // put request
  uint16_t put(const char *payload)
  {
    return send(COAP_CON, COAP_PUT, payload);
  }

  // post request
  uint16_t post(const char *payload)
  {
    return send(COAP_CON, COAP_POST, payload);
  }

  uint16_t subscribe()
  {
    return send(COAP_CON, COAP_PUT, "");
  }

  // observe request
  uint16_t observe()
  {
    uint8_t token = rand();
    return send(COAP_CON, COAP_GET, "", COAP_OBSERVE, &token);
  }

  uint16_t send(COAP_TYPE type, COAP_METHOD method, const char *payload, uint8_t observe = 0, uint8_t *token = NULL)
  {
    coapPacket packet;

    // make packet
    packet.type = type;
    packet.code = method;
    packet.token = token;
    packet.tokenlen = token == NULL ? 0 : sizeof(uint8_t);
    packet.payload = (uint8_t *)payload;
    packet.payloadlen = (uint32_t)strlen(payload);
    packet.optionnum = 0;
    packet.messageid = rand();

    if (observe)
    {
      packet.options[packet.optionnum].buffer = OBSERVE_REGISTER;
      packet.options[packet.optionnum].length = 0;
      packet.options[packet.optionnum].number = observe;
      packet.optionnum++;
    }

    if (method != COAP_EMPTY)
    {
      // options
      packet.options[packet.optionnum].buffer = (uint8_t *)url;
      packet.options[packet.optionnum].length = strlen(url);
      packet.options[packet.optionnum].number = COAP_URI_PATH;
      packet.optionnum++;
    }

    // send packet
    return sendPacket(packet);
  }

  uint16_t sendPacket(coapPacket &packet)
  {
    uint8_t buffer[BUF_MAX_SIZE];
    uint8_t *p = buffer;
    uint16_t running_delta = 0;
    uint16_t packetSize = 0;

    *p = COAP_VERSION << 6;

    *p |= (packet.type & 0x03) << 4;

    *p++ |= (packet.tokenlen & 0x0F);

    *p++ = packet.code;
    *p++ = (packet.messageid >> 8);
    *p++ = (packet.messageid & 0xFF);
    p = buffer + COAP_HEADER_SIZE;
    packetSize += 4;

    // make token
    if (packet.token != NULL && packet.tokenlen <= 0x0F)
    {
      memcpy(p, packet.token, packet.tokenlen);
      p += packet.tokenlen;
      packetSize += packet.tokenlen;
    }

    // make option header
    for (int i = 0; i < packet.optionnum; i++)
    {
      uint32_t optdelta;
      uint8_t len, delta;

      if (packetSize + 5 + packet.options[i].length >= BUF_MAX_SIZE)
      {
        return 0;
      }
      optdelta = packet.options[i].number - running_delta;
      COAP_OPTION_DELTA(optdelta, &delta);
      COAP_OPTION_DELTA((uint32_t)packet.options[i].length, &len);

      *p++ = (0xFF & (delta << 4 | len));
      if (delta == 13)
      {
        *p++ = (optdelta - 13);
        packetSize++;
      }
      else if (delta == 14)
      {
        *p++ = ((optdelta - 269) >> 8);
        *p++ = (0xFF & (optdelta - 269));
        packetSize += 2;
      }
      if (len == 13)
      {
        *p++ = (packet.options[i].length - 13);
        packetSize++;
      }
      else if (len == 14)
      {
        *p++ = (packet.options[i].length >> 8);
        *p++ = (0xFF & (packet.options[i].length - 269));
        packetSize += 2;
      }

      memcpy(p, packet.options[i].buffer, packet.options[i].length);
      p += packet.options[i].length;
      packetSize += packet.options[i].length + 1;
      running_delta = packet.options[i].number;
    }

    // make payload
    if (packet.payloadlen > 0)
    {
      if ((packetSize + 1 + packet.payloadlen) >= BUF_MAX_SIZE)
      {
        return 0;
      }
      *p++ = 0xFF;
      memcpy(p, packet.payload, packet.payloadlen);
      packetSize += 1 + packet.payloadlen;
    }

    if (this->hostName != nullptr)
    {
      udp.beginPacket(this->hostName, (uint16_t)this->port);
    }
    else
    {
      udp.beginPacket(this->ip, this->port);
    }
    udp.write(buffer, packetSize);
    udp.endPacket();

    return packet.messageid;
  }

  bool loop()
  {
    uint8_t buffer[BUF_MAX_SIZE];
    int32_t packetlen = udp.parsePacket();

    while (packetlen > 0)
    {
      packetlen = udp.read(buffer, packetlen >= BUF_MAX_SIZE ? BUF_MAX_SIZE : packetlen);

      coapPacket packet;

      // parse coap packet header
      if (packetlen < COAP_HEADER_SIZE || (((buffer[0] & 0xC0) >> 6) != 1))
      {
        packetlen = udp.parsePacket();
        continue;
      }

      packet.type = (buffer[0] & 0x30) >> 4;
      packet.tokenlen = buffer[0] & 0x0F;
      packet.code = buffer[1];
      packet.messageid = 0xFF00 & (buffer[2] << 8);
      packet.messageid |= 0x00FF & buffer[3];

      if (packet.tokenlen == 0)
        packet.token = NULL;
      else if (packet.tokenlen <= 8)
        packet.token = buffer + 4;
      else
      {
        packetlen = udp.parsePacket();
        continue;
      }

      // parse packet options/payload
      if (COAP_HEADER_SIZE + packet.tokenlen < packetlen)
      {
        int optionIndex = 0;
        uint16_t delta = 0;
        uint8_t *end = buffer + packetlen;
        uint8_t *p = buffer + COAP_HEADER_SIZE + packet.tokenlen;
        while (optionIndex < MAX_OPTION_NUM && *p != 0xFF && p < end)
        {
          if (0 != parseOption(&packet.options[optionIndex], &delta, &p, end - p))
            return false;
          optionIndex++;
        }
        packet.optionnum = optionIndex;

        if (p + 1 < end && *p == 0xFF)
        {
          packet.payload = p + 1;
          packet.payloadlen = end - (p + 1);
        }
        else
        {
          packet.payload = NULL;
          packet.payloadlen = 0;
        }
      }

      // when the server is restarted, re-subscibe
      if (packet.code == NOT_FOUND && packet.type == COAP_ACK)
      {
        subscribe();
      }
      // handle incoming data from server
      else if (packet.code == CONTENT && packet.type == COAP_CON && packet.payloadlen > 0)
      {
        // call response function
        resp(packet, udp.remoteIP(), udp.remotePort());

        packet.type = COAP_ACK;
        packet.code = COAP_EMPTY;
        packet.payload = NULL;
        packet.payloadlen = 0;
        packet.optionnum = 0;
        sendPacket(packet);
      }
      return true;
    }
    return false;
  }

  // parse option
  int parseOption(coapOption *option, uint16_t *running_delta, uint8_t **buf, size_t buflen)
  {
    uint8_t *p = *buf;
    uint8_t headlen = 1;
    uint16_t len, delta;

    if (buflen < headlen)
      return -1;

    delta = (p[0] & 0xF0) >> 4;
    len = p[0] & 0x0F;

    if (delta == 13)
    {
      headlen++;
      if (buflen < headlen)
        return -1;
      delta = p[1] + 13;
      p++;
    }
    else if (delta == 14)
    {
      headlen += 2;
      if (buflen < headlen)
        return -1;
      delta = ((p[1] << 8) | p[2]) + 269;
      p += 2;
    }
    else if (delta == 15)
      return -1;

    if (len == 13)
    {
      headlen++;
      if (buflen < headlen)
        return -1;
      len = p[1] + 13;
      p++;
    }
    else if (len == 14)
    {
      headlen += 2;
      if (buflen < headlen)
        return -1;
      len = ((p[1] << 8) | p[2]) + 269;
      p += 2;
    }
    else if (len == 15)
      return -1;

    if ((p + 1 + len) > (*buf + buflen))
      return -1;
    option->number = delta + *running_delta;
    option->buffer = p + 1;
    option->length = len;
    *buf = p + 1 + len;
    *running_delta += delta;

    return 0;
  }

  void response(callback c) { resp = c; }
};
#endif
