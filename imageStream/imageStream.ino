#include "ScreenControl.h"
#include <Bridge.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_RA8875.h>

#define STREAM_IMAGE        'P'
#define COMMAND_BUFFER_SIZE 1024

ScreenControl *sc = new ScreenControl();

char buff1[COMMAND_BUFFER_SIZE];
//char buff2[COMMAND_BUFFER_SIZE];
char *buff_ptr_read = buff1;
//char *buff_ptr_write = buff2;
//uint16_t pixelData[COMMAND_BUFFER_SIZE / 2];

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

void convert2PixelData(uint16_t *destData, char *srcData, int len) {
  int i;
  for(i = 0; i < len; i += 2) {
    destData[i / 2] = unpackNumber(srcData, i, 2);
  }
}
//
//void swapBuffers() {
//  char *swap = buff_ptr_write;
//  buff_ptr_write = buff_ptr_read;
//  buff_ptr_read = swap;
//}

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

int readPacket() {
//  Serial.println("buff_ptr_write addr: " + String((int) &buff_ptr_write));
//  Serial.println("buff_ptr_read addr: " + String((int) &buff_ptr_read));
  Serial1.readBytes(buff_ptr_read+1, 2);
  int packetSize = unpackNumber(buff_ptr_read, 1, 2);
  int bytesRead = 0;
  while(bytesRead < packetSize && Serial1.available() > 0) {
    bytesRead += Serial1.readBytes(buff_ptr_read + bytesRead + 3, packetSize-bytesRead);
  }
//  swapBuffers();
  return bytesRead;
}

// processes an image
void processImage() {
  while(Serial1.available() > 0) {
    memset(buff_ptr_read, 0x00, COMMAND_BUFFER_SIZE);
//    int numBytes = Serial1.readBytesUntil('\n', ret, COMMAND_BUFFER_SIZE);
    int numBytes;
    Serial1.readBytes(buff_ptr_read, 1);
//    Serial.println("number of bytes: " + String(numBytes));
    switch(buff_ptr_read[0]) {
      case STREAM_IMAGE:
        Serial.println("stream image");
        numBytes = readPacket();
//        Serial.println(String(numBytes));
//        printData(buff_ptr_read, numBytes + 2);
//        convert2PixelData(pixelData, buff_ptr_read + 11, numBytes - 8);
//        sc->drawImage(pixelData, (numBytes - 8) / 2, unpackNumber(buff_ptr_read, 3, 2), unpackNumber(buff_ptr_read, 5, 2),
        sc->drawImage((uint16_t *) (buff_ptr_read + 11), (numBytes - 8) / 2, unpackNumber(buff_ptr_read, 3, 2), unpackNumber(buff_ptr_read, 5, 2),
                      unpackNumber(buff_ptr_read, 7, 2), unpackNumber(buff_ptr_read, 9, 2));
        Serial1.println(String(STREAM_IMAGE));
        break;
      default:
        Serial.println("Error: message command not recognized");
        Serial1.flush();
//        printData(ret, numBytes);
    }
  }
}

void setup() {
  Serial.begin(9600);
  Serial1.begin(115200);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin();
  if(!sc->init()) {
    while(true) {
      Serial.println("Error");
      delay(2000);
    }
  }
  digitalWrite(13, HIGH);
}

void loop() {
  processImage();
  sc->processTouch();
}
    
    
