

#include "TempControl.h"

// Constructor for the TempControl class
// Sets a target temperature and mode
TempControl::TempControl() {
  Serial.println("Creating new temperature controller");
  targetTemp = DEFAULT_TARGET_TEMP;
  roomTemp = 65; // TODO remove this
  isSystemOn = false;
  mode = DEFAULT_MODE;
  unit = DEFAULT_UNIT;
  lastTurnTime = lastProcessTime = 0;
  fanOverride = OFF;
}

// Returns the current temperature of the room
int TempControl::getRoomTemp() {
  return roomTemp;
}

// Returns the target temperature
int TempControl::getTargetTemp() {
  return targetTemp;
}

// Returns the current mode of operation
uint8_t TempControl::getMode() {
  return mode;
}

// Returns the temperature units
uint8_t TempControl::getUnit() {
  return unit;
}

// Returns true is the system is on, false otherwise
bool TempControl::isOn() {
  return isSystemOn;
}

// Sets the target temperature
void TempControl::setTargetTemp(int temp) {
  targetTemp = temp;
}

// Lowers the temperature by one unit
void TempControl::incrementTargetTemp() {
  targetTemp++;
}

// Raises the target temperature by one unit
void TempControl::decrementTargetTemp() {
  targetTemp--;
}

// Switches the system between heating and cooling
// Turns the system off before switching modes
void TempControl::switchMode() {
  turn(OFF);
  switch(mode) {
    case HEATING:
      mode = COOLING;
      break;
    case COOLING:
      mode = HEATING;
      break;
    default:
      //TODO Log an error!
      ;
   }
}

// Swtiches the temperature units
void TempControl::switchUnit() {
  switch(unit) {
    case FAHRENHEIT:
      unit = CELCIUS;
      roomTemp = F2C(roomTemp);
      targetTemp = F2C(targetTemp);
      break;
    case CELCIUS:
      unit = FAHRENHEIT;
      roomTemp = C2F(roomTemp);
      targetTemp = C2F(targetTemp);
      break;
    default:
      //TODO Log an error!
      ;
  }
}

// Retrieves the room temperature reading and determines whether the
// system should be on or off based on the target temperature.
// This only processes the temperature every PROCESS_TIME.
void TempControl::processTemperature() {
  if((millis() - lastProcessTime) >= PROCESS_TIME) {
    roomTemp = READTEMP;
    isSystemOn = (IS_ON(COOL_PIN)) || (IS_ON(HEAT_PIN));
    switch(mode) {
      case HEATING:
        if(roomTemp <= targetTemp && !isSystemOn) {
          turn(ON);
        } else if(roomTemp > targetTemp && isSystemOn) {
          turn(OFF);
        }
        break;
      case COOLING:
        if(roomTemp >= targetTemp && !isSystemOn) {
          turn(ON);
        } else if(roomTemp < targetTemp && isSystemOn) {
          turn(OFF);
        }
        break;
      default:
        //TODO Log and error!
        ;
    }
    lastProcessTime = millis();
  }
}

// Turns on and off the heating or cooling system
// Checks that heating and cooling are never on at the same time
// Disables systems from turning back on within a minute
void TempControl::turn(uint8_t on_or_off) {
  if((on_or_off == OFF) || ((millis() - lastTurnTime) >= ON_TIME_DELAY)) {
    switch(mode) {
      case HEATING:
        if(IS_ON(COOL_PIN)) {
          COOL(OFF);
          //TODO Log an error!
        }
        HEAT(on_or_off);
        break;
      case COOLING:
        if(IS_ON(HEAT_PIN)) {
          HEAT(OFF);
          //TODO Log an error! 
        }
        COOL(on_or_off);
        break;
      default:
        //TODO Log an error!
        ;
    }
    lastTurnTime = millis();
    //TODO Log that the system turned on
  }
}

// Alternative digitalRead function
// Should this really be part of this class?
int TempControl::digitalReadAlt(int pin) const {
  return ((*portOutputRegister(digitalPinToPort(pin)) & digitalPinToBitMask(pin)) ? HIGH : LOW);
}

