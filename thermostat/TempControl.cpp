

#include "TempControl.h"

// Constructor for the TempControl class
// Sets a target temperature and mode
TempControl::TempControl() {
//  Serial.println("Creating new temperature controller");
  targetTemp = DEFAULT_TARGET_TEMP * TEMP_MULTIPLE;
  roomTemp = DEFAULT_ROOM_TEMP * TEMP_MULTIPLE; // TODO remove this
  isSystemOn = false;
  mode = DEFAULT_MODE;
  unit = DEFAULT_UNIT;
  lastTurnTime, lastTempUpdateTime = 0;
  fanOverride = OFF;
}

// Returns the current temperature of the room
int TempControl::getRoomTemp() {
  return roomTemp / TEMP_MULTIPLE;
}

// Returns the target temperature
int TempControl::getTargetTemp() {
  return targetTemp / TEMP_MULTIPLE;
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
  targetTemp = temp * TEMP_MULTIPLE;
}

// Lowers the temperature by one unit
void TempControl::incrementTargetTemp() {
  targetTemp += TEMP_MULTIPLE;
}

// Raises the target temperature by one unit
void TempControl::decrementTargetTemp() {
  targetTemp -= TEMP_MULTIPLE;
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
      targetTemp = F2C(targetTemp);
      break;
    case CELCIUS:
      unit = FAHRENHEIT;
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
  if((millis() - lastTempUpdateTime) >= TEMP_UPDATE_DELAY) {
    roomTemp = convertRawTemp(READTEMP); // TODO filter the temperature readings?
//    Serial.println(getRoomTemp());
//    roomTemp = 65;
    isSystemOn = (IS_ON(COOL_PIN)) || (IS_ON(HEAT_PIN));
//    Serial.println(isSystemOn);
    switch(mode) {
      case HEATING:
        if(roomTemp <= targetTemp && !isSystemOn) {
          turn(ON);
          Serial.println("heat turned on!");
        } else if(roomTemp > targetTemp && isSystemOn) {
          turn(OFF);
          Serial.println("heat turned off!");
        }
        break;
      case COOLING:
        if(roomTemp >= targetTemp && !isSystemOn) {
          turn(ON);
          Serial.println("cooling turned on!");
        } else if(roomTemp < targetTemp && isSystemOn) {
          turn(OFF);
          Serial.println("cooling turned off!");
        }
        break;
      default:
        //TODO Log and error!
        ;
    }
    lastTempUpdateTime = millis();
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
    //TODO Log that the system turned on or off
  }
}

// Alternative digitalRead function
// Should this really be part of this class?
int TempControl::digitalReadAlt(int pin) const {
  return ((*portOutputRegister(digitalPinToPort(pin)) & digitalPinToBitMask(pin)) ? HIGH : LOW);
}

int TempControl::convertRawTemp(int raw) {
  int calc = lround((1023 * CALC_MULTIPLE * TEMP_SHIFT) / \
            ((COMP_RESISTOR) /  \
              (BASE_RESISTANCE * exp(THERM_B * \
                (((1 * TEMP_MULTIPLE) / (raw + KELVIN_OFFSET * TEMP_MULTIPLE)) - \
                ((1) / (RES_THERM_NOM + KELVIN_OFFSET))))) + \
              (1)) / CALC_MULTIPLE) + CELCIUS_OFFSET * TEMP_MULTIPLE; // TODO remove celcius offset?
  return (unit == CELCIUS) ? calc : C2F(calc);
}
