#include <Process.h>
#include "MessagePassing.h"

void runRandomMessageTest() {
  Process p;
  p.begin("python");
  p.addParameter("/mnt/sda1/arduino/randomMessageTest.py");
  p.runAsynchronously();
}

void setup() {
  // put your setup code here, to run once:
   Serial.begin(9600);
   Serial1.begin(250000);
   pinMode(13, OUTPUT);
   digitalWrite(13, LOW);
   Bridge.begin();
   digitalWrite(13, HIGH);
   
   delay(2000); // wait 2 seconds
}

void loop() {
  // put your main code here, to run repeatedly:
  //runRandomMessageTest();
  processCommands();
  delay(2000);
}
