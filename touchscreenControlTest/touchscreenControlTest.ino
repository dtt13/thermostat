#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_RA8875.h>
#include "ScreenControl.h"

ScreenControl sc = ScreenControl();
//Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RESET);

void setup() {
  // put your setup code here, to run once:
   Serial.begin(9600);
   pinMode(13, OUTPUT);
   digitalWrite(13, LOW);
   if(!sc.init()) {
     while(1);
  }
  digitalWrite(13,HIGH);
}

void loop() {
  sc.processTouch();
}
