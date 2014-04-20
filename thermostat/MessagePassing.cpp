
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
//    = Serial1.readBytesUntil('\n', ret, COMMAND_BUFFER_SIZE);
    Serial1.readBytes(buff_ptr, 1);
//    Serial.println("number of bytes read: " + String(numBytes));
    switch(buff_ptr[0]) {
      case GET_ROOM_TEMP:
//        Serial.println("get room temperature");
        numBytes = readPacket();
        Serial1.println(String(GET_ROOM_TEMP) + String(tc->getRoomTemp()));
        break;
      case GET_TARGET_TEMP:
//        Serial.println("get target temperature");
        numBytes = readPacket();
        Serial1.println(String(GET_TARGET_TEMP) + String(tc->getTargetTemp()));
        break;
      case GET_MODE:
//        Serial.println("get mode");
        numBytes = readPacket();
        Serial1.println(String(GET_MODE) + modeToString(tc));
        break;
      case GET_UNIT:
//        Serial.println("get unit");
        numBytes = readPacket();
        Serial1.println(String(GET_UNIT) + unitToString(tc));
        break;
      case IS_ON:
//        Serial.println("is on?");
        numBytes = readPacket();
        Serial1.println(String(IS_ON) + isOnToString(tc));
        break;
      case SET_TARGET_TEMP:
//        Serial.println("set target temperature");
        numBytes = readPacket();
        tc->setTargetTemp(unpackNumber(buff_ptr, 3, 2));
        Serial1.println(String(SET_TARGET_TEMP));
        break;
      case SWITCH_MODE:
//        Serial.println("switch mode");
        numBytes = readPacket();
        tc->switchMode();
        Serial1.println(String(SWITCH_MODE));
        break;
      case SWITCH_UNIT:
//        Serial.println("switch unit");
        numBytes = readPacket();
        tc->switchUnit();
        Serial1.println(String(SWITCH_UNIT));
        break;
      case WRITE_TEXT:
//        Serial.println("write text");
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
//        Serial.println("stream image");
        numBytes = readPacket();
        sc->hideApp(true, true);
        sc->layerMode(2);
//        Serial.println("read the packet");
        sc->drawImage((uint16_t *) (buff_ptr + 11), (numBytes - 8) / 2, unpackNumber(buff_ptr, 3, 2), unpackNumber(buff_ptr, 5, 2),
                      unpackNumber(buff_ptr, 7, 2), unpackNumber(buff_ptr, 9, 2));
        Serial1.println(String(STREAM_IMAGE));
        sc->layerMode(1);
        lastStreamTime = millis();
        isStreaming = true;
        break;
      default:
        Serial.println("Error: message command not recognized");
        numBytes = Serial1.readBytes(buff_ptr, COMMAND_BUFFER_SIZE);
//        printData(buff_ptr, numBytes);
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
      return "heating";
    case CELCIUS:
      return "cooling";
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
    return "true";
  } else {
    return "false";
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
