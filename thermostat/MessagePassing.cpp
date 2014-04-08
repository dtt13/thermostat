
#include "MessagePassing.h"

char ret[COMMAND_BUFFER_SIZE];

// Interprets commands from Linino for controlling the thermostat and touchscreen
void processCommands(TempControl *tc, ScreenControl *sc) {
  
//  Serial.println("processing...");
  while(Serial1.available() > 0) {
    memset(ret, 0x00, sizeof(ret));
    int numBytes = Serial1.readBytesUntil('\n', ret, COMMAND_BUFFER_SIZE);
    Serial.println("number of bytes read: " + String(numBytes));
    switch(ret[0]) {
      case GET_ROOM_TEMP:
        Serial.println("get room temperature");
        Serial1.println(String(GET_ROOM_TEMP) + String(tc->getRoomTemp()));
        break;
      case GET_TARGET_TEMP:
        Serial.println("get target temperature");
        Serial1.println(String(GET_TARGET_TEMP) + String(tc->getTargetTemp()));
        break;
      case GET_MODE:
        Serial.println("get mode");
        Serial1.println(String(GET_MODE) + modeToString(tc));
        break;
      case GET_UNIT:
        Serial.println("get unit");
        Serial1.println(String(GET_UNIT) + unitToString(tc));
        break;
      case IS_ON:
        Serial.println("is on?");
        Serial1.println(String(IS_ON) + isOnToString(tc));
        break;
      case SET_TARGET_TEMP:
        Serial.println("set target temperature");
        tc->setTargetTemp(unpackNumber(ret, 1, 2));
        Serial1.println(String(SET_TARGET_TEMP));
        break;
      case SWITCH_MODE:
        Serial.println("switch mode");
        tc->switchMode();
        Serial1.println(String(SWITCH_MODE));
        break;
      case SWITCH_UNIT:
        Serial.println("switch unit");
        tc->switchUnit();
        Serial1.println(String(SWITCH_UNIT));
        break;
      case WRITE_TEXT:
        Serial.println("write text");
        Serial1.println(String(WRITE_TEXT));
        ret[numBytes] = '\0';
        sc->layerMode(2);
        sc->writeText(unpackNumber(ret, 1, 2), unpackNumber(ret, 3, 2),
                            unpackNumber(ret, 5, 2), unpackNumber(ret, 7, 2),
                            unpackNumber(ret, 9, 1), ret+10);
        sc->layerMode(1);
        break;
//      case STREAM_IMAGE:
//        Serial.println("stream image");
//        imageInitResponse();
//        receiveImage(unpackNumber(ret, 3, 2), unpackNumber(ret, 5, 2),
//                      unpackNumber(ret, 7, 2), unpackNumber(ret, 9, 2));
//        break;
      default:
        Serial.println("Error: message command not recognized");
        Serial.println((int)ret, BIN);
    }
  }
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

// Processes the image and forwards the data to the screen
//void receiveImage(uint16_t xpos, uint16_t ypos, uint16_t width, uint16_t height) {
//  // allocate the buffer and start receiving data
//  char image[IMAGE_BUFFER_SIZE];
//  uint8_t failCount = 0;
////  bool success = false;
//  uint32_t pixelCount = 0;
//  Serial.println(String(width) + ", " + String(height));
//  uint32_t pixelTotal = (uint32_t)(width * height);
//  Serial.println("pixelTotal: " + String(pixelTotal));
//  while((failCount < MAX_FAIL_COUNT) && (pixelCount < pixelTotal)) {
//    // read next packet
//    Serial.println("in the loop");
//    memset(image, 0x00, IMAGE_BUFFER_SIZE);
//    int bytesRead = Serial1.readBytes(image, IMAGE_BUFFER_SIZE);
//    printData(image, bytesRead);//TODO remove this; only prints packet contents
//    // interpret packet
//    if(image[0] == STREAM_IMAGE) {
//      switch(image[1]) {
//        case STREAM_INIT:
//          Serial.println("init");
//          imageInitResponse();
//          failCount = 0;
//          break;
//        case STREAM_SEND:
//          Serial.println("send");
//          pixelCount += imageSendResponse(image, bytesRead);
//          break;
//        default:
//          failCount++;
//      }
//    } else {
//      failCount++;
//    }
//  }
//  Serial.println("pixelCount: " + String(pixelCount));
//}

void printData(char *data, int len) {
  int i;
//  Serial.println("read bytes: " + String(bytesRead));
  for(i = 0; i < len; i++) {
    if(uint8_t(data[i]) < 0x10) {
      Serial.print("0");
    }
    Serial.print((0x00FF & data[i]), HEX);
  }
  Serial.println();
}

uint32_t imageSendResponse(char *image, int len) {
  uint32_t pixelCount = (uint32_t)((len - 2) / 2);
  imageToScreen(image+2, len-2);
  Serial1.println(STREAM_IMAGE);
  return pixelCount;
}

void imageCopy(char* dest, char* src, int len) {
  int i;
  for(i = 0; i < len; i++) {
    dest[i] = src[i];
  }
}

// Writes pixels to the touchscreen
void imageToScreen(char *image, int len) {
  printData(image, len); // TODO actually right to screen
}

// Sends an init response to begin image streaming
void imageInitResponse() {
  char responseInit[5] = {STREAM_IMAGE, (IMAGE_BUFFER_SIZE & 0xFF00) >> 8, (IMAGE_BUFFER_SIZE & 0x00FF), 0, 0};
  writeResponse(responseInit, sizeof(responseInit));
}

//// Sends a send response for image streaming
//uint16_t imageSendResponse(char *image, int len, uint16_t pixelCount) {
//  char responseSend[3] = {STREAM_IMAGE, 0, 0};
//  uint16_t pixelsRead = 0; // grouped in 8s
//  int byteCount = 0;
//  bool finished = false;
//  while(!finished && (byteCount < len) && (image[byteCount] == STREAM_IMAGE) && (image[byteCount + 1] == STREAM_SEND)) {
//    int pixelsTemp = imageToScreen(image + byteCount, len - byteCount, pixelCount + pixelsRead);
//    if(pixelsTemp != 0) {
//      pixelsRead += pixelsTemp;
//      byteCount += pixelsTemp * 16 + 6;
//    } else {
//      finished = true;
//    }
//  }
//  responseSend[1] = (0xFF00 & (pixelCount + pixelsRead)) >> 8;
//  responseSend[2] = (0x00FF & (pixelCount + pixelsRead));
//  writeResponse(responseSend, sizeof(responseSend));
//  return pixelsRead;
//}

// Sends a response to the Linino on Serial1
void writeResponse(char *response, int len) {
  int i;
  for(i = 0; i < len; i++) {
    Serial1.print(response[i]);
  }
  Serial1.println();
}

//// Processes a single packet and sends pixel data to the screen
//// Returns the number of pixels actually written
//uint16_t imageToScreen(char *image, int len, uint16_t pixelCount) {
//  uint16_t firstPixel = unpackNumber(image, 2, 2);
//  uint16_t pixelsInPack = unpackNumber(image, 4, 2);
////  Serial.println(String(pixelsInPack) + ", " + String(bytesRead));
////  Serial.println(String(pixelsInPack * 16 + 6));
//  if((pixelsInPack * 16 + 6 > len) || (firstPixel != pixelCount)) { // TODO grab the end of the packet if necessary
//    return 0;
//  } else {
//    return pixelsInPack;
//  }
//}


