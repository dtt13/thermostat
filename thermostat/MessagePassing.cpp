
#include "MessagePassing.h"

char buff[COMMAND_BUFFER_SIZE];
char *buff_ptr = buff;
bool isStreaming = false;
long lastStreamTime = 0;

// Interprets commands from Linino for controlling the thermostat and touchscreen
void processCommands(TempControl *tc, ScreenControl *sc) {
  if(isStreaming && (millis() - lastStreamTime) > STREAM_DELAY) {
    isStreaming = false;
    sc->hideApp(false, false);
  }
//  Serial.println("processing...");
  while(Serial1.available() > 0) {
    memset(buff_ptr, 0x00, COMMAND_BUFFER_SIZE);
    int numBytes;
    Serial1.readBytes(buff_ptr, 1);
//    Serial.println("number of bytes read: " + String(numBytes));
    switch(buff_ptr[0]) {
      case GET_TEMP:
        numBytes = readPacket();
        buff_ptr[6] = tc->getRoomTemp();
        buff_ptr[8] = tc->getTargetTemp();
        Serial1.print(String(GET_TEMP));
        Serial1.write((uint8_t *)(buff_ptr + 6), 4);
        Serial1.println();
        break;
      case GET_MODE:
        numBytes = readPacket();
        Serial1.println(String(GET_MODE) + modeToString(tc));
        break;
      case GET_UNIT:
        numBytes = readPacket();
        Serial1.println(String(GET_UNIT) + unitToString(tc));
        break;
      case IS_ON:
        numBytes = readPacket();
        Serial1.println(String(IS_ON) + isOnToString(tc));
        break;
      case SET_TARGET_TEMP:
        numBytes = readPacket();
        tc->setTargetTemp(unpackNumber(buff_ptr, 3, 2));
        Serial1.println(String(SET_TARGET_TEMP));
        break;
      case SWITCH:
        numBytes = readPacket();
        Serial1.println(String(SWITCH));
        switch(buff_ptr[3]) {
          case SWITCH_MODE:
            tc->switchMode();
            break;
          case SWITCH_UNIT:
            tc->switchUnit();
            break;
          case SWITCH_FAN:
            tc->switchFan();
            break;
        }
      case WRITE_TEXT:
        numBytes = readPacket();
        Serial1.println(String(WRITE_TEXT));
        buff_ptr[numBytes + 3] = '\0';
        sc->layerMode(2);
        sc->writeText(unpackNumber(buff_ptr, 3, 2), unpackNumber(buff_ptr, 5, 2),
                            unpackNumber(buff_ptr, 7, 2), unpackNumber(buff_ptr, 9, 2),
                            unpackNumber(buff_ptr, 11, 1), buff_ptr+12);
        sc->layerMode(1);
        break;
      case STREAM_IMAGE:
        numBytes = readPacket();
        sc->hideApp(true, true);
        sc->layerMode(2);
        sc->drawImage((uint16_t *) (buff_ptr + 11), (numBytes - 8) / 2, unpackNumber(buff_ptr, 3, 2), unpackNumber(buff_ptr, 5, 2),
                      unpackNumber(buff_ptr, 7, 2), unpackNumber(buff_ptr, 9, 2));
        Serial1.println(String(STREAM_IMAGE));
        sc->layerMode(1);
        lastStreamTime = millis();
        if(!isStreaming) {
          isStreaming = true;
        }
        break;
      case CLEAR_APP:
        numBytes = readPacket();
        sc->clearApp();
        Serial1.println(String(CLEAR_APP));
        break;
      case SET_IP:
        numBytes = readPacket();
        strcpy(sc->ipaddr, buff_ptr + 3);
        Serial1.println(String(SET_IP));
      default:
        Serial.println("Error: message command not recognized");
        numBytes = Serial1.readBytes(buff_ptr, COMMAND_BUFFER_SIZE);
    }
  }
}

int readPacket() {
  Serial1.readBytes(buff_ptr+1, 2);
  int packetSize = unpackNumber(buff_ptr, 1, 2);
  int bytesRead = 0;
  while(bytesRead < packetSize && Serial1.available() > 0) {
//    Serial.println("bytes read: " + String(bytesRead));
    bytesRead += Serial1.readBytes(buff_ptr + bytesRead + 3, packetSize-bytesRead);
  }
  return bytesRead;
}

// Returns a String representation of the thermostat's mode 
String modeToString(TempControl *tc) {
  switch(tc->getMode()) {
    case HEATING:
      return "h";
    case CELCIUS:
      return "c";
  }
  return "";
}

// Returns a String representation of the current units of the thermostat
String unitToString(TempControl *tc) {
  switch(tc->getUnit()) {
    case FAHRENHEIT:
      return "f";
    case CELCIUS:
      return "c";
  }
  return "";
}

// Returns a String representation of the whether the thermostat is on or off
String isOnToString(TempControl *tc) {
  if(tc->isOn()) {
    if(tc->isFanOn()) {
      return "tt";
    }
    return "tf";
  } else {
    if(tc->isFanOn()) {
      return "ft";
    }
    return "ff";
  }
}

// Grabs the number representation of the a sequence of bytes
uint16_t unpackNumber(char *bytes, int start, int len) {
  uint16_t output = 0;
  int i;
  for(i = start; i < start + len; i++) {
    output = output << 8;
    output = output | (0x00FF & bytes[i]);
  }
  return output;
}
