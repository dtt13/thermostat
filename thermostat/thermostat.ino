
#include "MessagePassing.h"
#include "TempControl.h"
#include "ScreenControl.h"
//#include <Console.h>
#include <Bridge.h>
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <Adafruit_RA8875.h>

TempControl *tc = new TempControl();
ScreenControl *sc = new ScreenControl(tc);

// processes interrupts from touch screen
void touchISR() {
  sc->touchFlag = true;
  Serial.println("You touched the screen");
}

// initialization code
void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial1.setTimeout(1000);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin(); // TODO maybe needs replacing?
  pinMode(RA8875_INT, INPUT);
  digitalWrite(RA8875_INT, HIGH);
//  attachInterrupt(0, touchISR, FALLING);
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
//  tc->processTemperature();
  }
}
