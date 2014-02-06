

#include "TempControl.h"
// Constructor for the TemperatureControl class
// Sets a target temperature and mode
TemperatureControl::TemperatureControl(int t, uint8_t m) {
  targetTemp = t;
  currentTemp = READTEMP;
  mode = m;
  lastTurnTime = 0;
  fanOverride = OFF;
}

// Returns the current temperature of the room
int TemperatureControl::getCurrentTemp() {
  return currentTemp;
}

// Returns the target temperature
int TemperatureControl::getTargetTemp() {
  return targetTemp;
}

// Returns the current mode of operation
uint8_t TemperatureControl::getMode() {
  return mode;
}

// Sets the target temperature
void TemperatureControl::setTargetTemp(int temp) {
  targetTemp = temp;
}

// Switches the system between heating and cooling
// Turns the system off before switching modes
void TemperatureControl::switchMode() {
  turn(OFF);
  switch(mode) {
    case HEATING:
      mode = COOLING;
      break;
    case COOLING:
      mode = HEATING;
      break;
    default:
      //Log an error!
      ;
   }
}

// Turns on and off the heating or cooling system
// Checks that heating and cooling are never on at the same time
// Disables systems from turning back on within a minute
void TemperatureControl::turn(uint8_t on_or_off) {
  if((on_or_off == OFF) || ((millis() - lastTurnTime) >= ON_TIME_DELAY)) {
    switch(mode) {
      case HEATING:
        if(IS_ON(COOL_PIN)) {
          COOL(OFF);
          //Log an error!
        }
        HEAT(on_or_off);
        break;
      case COOLING:
        if(IS_ON(HEAT_PIN)) {
          HEAT(OFF);
          //Log an error! 
        }
        COOL(on_or_off);
        break;
      default:
        //Log an error!
        ;
    }
    lastTurnTime = millis();
    //Log that the system turned on
  }
}

// Alternative digitalRead function
// Should this really be part of this class?
int TemperatureControl::digitalReadAlt(int pin) const {
  return ((*portOutputRegister(digitalPinToPort(pin)) & digitalPinToBitMask(pin)) ? HIGH : LOW);
}

