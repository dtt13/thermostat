
#include "MessagePassing.h"
#include "TempControl.h"
#include "ScreenControl.h"
#include <Process.h>
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <Adafruit_RA8875.h>

TempControl *tc = new TempControl();
ScreenControl sc = ScreenControl(tc);

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin();
  if(!sc.init()) {
    while(1);
  }
  digitalWrite(13,HIGH);
}

void loop() {
  processCommands(tc);
  sc.processTouch();
}
