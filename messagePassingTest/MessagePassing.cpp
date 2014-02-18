

#include "MessagePassing.h"

// Interprets commands from Linino for controlling the thermostat and touchscreen
void processCommands() {
  char ret[8];
//  Serial.println("processing...");
//  Serial.println(Serial1.available());
  while(Serial1.available() > 0) {
    memset(ret, 0x00, sizeof(ret));
    
    Serial1.readBytesUntil('\n', ret, 8); //TODO: change this from 2
    switch(ret[0]) {
      case SET_TARGET_TEMP:
        Serial.println("set target temperature");
        Serial1.println("S");
        break;
      case GET_TARGET_TEMP:
        Serial.println("get target temperature");
        Serial1.println("G81");
        break;
      case GET_ROOM_TEMP:
        Serial.println("get room temperature");
        Serial1.println("R79");
        break;
      case INCREMENT_TARGET_TEMP:
        Serial.println("increment target temperature");
        Serial1.println("I");
        break;
      case DECREMENT_TARGET_TEMP:
        Serial.println("decrement target temperature");
        Serial1.println("D");
        break;
      default:
        Serial.println("Error: message command not recognized");
        Serial.println(ret);
    }
  }
}
