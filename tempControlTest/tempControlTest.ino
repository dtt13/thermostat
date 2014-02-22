#include "TempControl.h" 

TempControl tc = TempControl.getInstance();

void setup() {
  Serial.begin(9600);
  while(!Serial);
  
  Serial.println("Starting temperature controller...");
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  delay(2000); // wait 2 seconds
  digitalWrite(13, HIGH);
}

void loop() {
  Serial.println("current temp:");
  Serial.println(tc.getCurrentTemp());
  Serial.println("target temp: ");
  Serial.println(tc.getTargetTemp());
  delay(5000); // wait 5 seconds
  tc.setTargetTemp(tc.getTargetTemp() / 2);
}
