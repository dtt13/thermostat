#include <Process.h>
#include "MessagePassing.h"
#include "TempControl.h"

TempControl *tc = new TempControl();
void setup() {
  // put your setup code here, to run once:
   Serial.begin(9600);
   Serial1.begin(9600);
   pinMode(13, OUTPUT);
   digitalWrite(13, LOW);
   Bridge.begin();
   digitalWrite(13, HIGH);
   delay(2000); // wait 2 seconds
}

void loop() {
  // put your main code here, to run repeatedly:
  processCommands(tc);
}
