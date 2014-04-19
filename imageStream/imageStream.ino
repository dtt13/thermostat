#include "ScreenControl.h"
#include <Bridge.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_RA8875.h>

#define STREAM_IMAGE        'P'
#define MESSAGE_FAIL        'X'
#define COMMAND_BUFFER_SIZE 1024

ScreenControl *sc = new ScreenControl();

char buff1[COMMAND_BUFFER_SIZE];
char *buff_ptr_read = buff1;

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
    int numBytes;
    Serial1.readBytes(buff_ptr_read, 1);
    switch(buff_ptr_read[0]) {
      case STREAM_IMAGE:
        Serial.println("stream image");
        numBytes = readPacket();
        sc->drawImage((uint16_t *) (buff_ptr_read + 11), (numBytes - 8) / 2, unpackNumber(buff_ptr_read, 3, 2), unpackNumber(buff_ptr_read, 5, 2),
                      unpackNumber(buff_ptr_read, 7, 2), unpackNumber(buff_ptr_read, 9, 2));
        Serial1.println(String(STREAM_IMAGE));
        break;
      default:
        Serial.println("Error: message command not recognized");
        numBytes = Serial1.readBytes(buff_ptr_read, COMMAND_BUFFER_SIZE);
        printData(buff_ptr_read, numBytes);
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
    
    
