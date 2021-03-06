
#include "MessagePassing.h"
#include "TempControl.h"
#include "ScreenControl.h"
//#include <Console.h>
#include <Bridge.h>
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <Adafruit_RA8875.h>

TempControl *tc = new TempControl();
ScreenControl *sc = 
new ScreenControl(tc);

// initialization code
void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin(); // TODO maybe needs replacing?
  if(!sc->init()) {
    while(true) {
      Serial.println("Error");
      delay(2000);
    }
  }
  digitalWrite(13, HIGH);
}

// main loop
void loop() {
  processCommands(tc, sc);
  if(!isStreaming) {
    sc->processTouch();
    tc->processTemperature();
  }
}
