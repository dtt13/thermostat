

#include "MessagePassing.h"

// Interprets commands from Linino for controlling the thermostat and touchscreen
void processCommands() {
  char ret[8];
//  Serial.println("processing...");
//  Serial.println(Serial1.available());
  int targetTemp = 80;
  int roomTemp = 65;
  while(Serial1.available() > 0) {
    memset(ret, 0x00, sizeof(ret));
    Serial1.readBytesUntil('\n', ret, 8);
    switch(ret[0]) {
      case SET_TARGET_TEMP:
        Serial.println("set target temperature");
        Serial1.println(String(SET_TARGET_TEMP));
        break;
      case GET_TARGET_TEMP:
        Serial.println("get target temperature");
        Serial1.println(String(GET_TARGET_TEMP) + String(targetTemp));
        break;
      case GET_ROOM_TEMP:
        Serial.println("get room temperature");
        Serial1.println(String(GET_ROOM_TEMP) + String(roomTemp));
        break;
      case INCREMENT_TARGET_TEMP:
        Serial.println("increment target temperature");
        Serial1.println(String(INCREMENT_TARGET_TEMP));
        break;
      case DECREMENT_TARGET_TEMP:
        Serial.println("decrement target temperature");
        Serial1.println(String(DECREMENT_TARGET_TEMP));
        break;
      default:
        Serial.println("Error: message command not recognized");
        Serial.println(ret);
    }
  }
}
