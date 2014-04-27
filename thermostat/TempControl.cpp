

#include "TempControl.h"

// Constructor for the TempControl class
// Sets a target temperature and mode
TempControl::TempControl() {
  targetTemp = DEFAULT_TARGET_TEMP * TEMP_MULTIPLE;
  roomTemp = DEFAULT_ROOM_TEMP * TEMP_MULTIPLE;
  isSystemOn = false;
  mode = DEFAULT_MODE;
  unit = DEFAULT_UNIT;
  lastTurnTime, lastTempUpdateTime = 0;
  fanOnOverride = OFF;
  pinMode(COOL_PIN, OUTPUT);
  pinMode(HEAT_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  COOL(OFF);
  HEAT(OFF);
}

// Returns the current temperature of the room
int TempControl::getRoomTemp() {
  return roomTemp / TEMP_MULTIPLE;
}

// Returns the target temperature
int TempControl::getTargetTemp() {
  return targetTemp / TEMP_MULTIPLE ;
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

// Returns true if the fan is on, false otherwise
bool TempControl::isFanOn() {
  return IS_ON(FAN_PIN);
}

// Sets the target temperature
void TempControl::setTargetTemp(int temp) {
  targetTemp = temp * TEMP_MULTIPLE;
  coerceTargetTemp();
}

// Lowers the temperature by one unit
void TempControl::incrementTargetTemp() {
  targetTemp += TEMP_MULTIPLE;
  coerceTargetTemp();
}

// Raises the target temperature by one unit
void TempControl::decrementTargetTemp() {
  targetTemp -= TEMP_MULTIPLE;
  coerceTargetTemp();
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
      ;
   }
}

// Switches the fan on or off
void TempControl::switchFan() {
  switch(fanOnOverride) {
    case ON:
      fanOnOverride = OFF;
      break;
    case OFF:
      fanOnOverride = ON;
      break;
    default:
      ;
  }
  FAN(digitalReadAlt(COOL_PIN));
}

// Swtiches the temperature units
void TempControl::switchUnit() {
  switch(unit) {
    case FAHRENHEIT:
      unit = CELCIUS;
      targetTemp = convertTemp(targetTemp, CELCIUS);
      break;
    case CELCIUS:
      unit = FAHRENHEIT;
      targetTemp = convertTemp(targetTemp, FAHRENHEIT);
      break;
    default:
      ;
  }
  coerceTargetTemp();
}

// Forces the target temperature to be between 10 and 100
void TempControl::coerceTargetTemp() {
  int temp = getTargetTemp();
  if(temp > 99) {
    targetTemp = 99 * TEMP_MULTIPLE;
  } else if(temp < 10) {
    targetTemp = 10 * TEMP_MULTIPLE;
  }
}

// Retrieves the room temperature reading and determines whether the
// system should be on or off based on the target temperature.
// This only processes the temperature every PROCESS_TIME.
void TempControl::processTemperature() {
  if((millis() - lastTempUpdateTime) >= TEMP_UPDATE_DELAY) {
      captureRoomTemp();
//    roomTemp = 65 * TEMP_MULTIPLE;
    isSystemOn = (IS_ON(COOL_PIN)) || (IS_ON(HEAT_PIN));
    switch(mode) {
      case HEATING:
        if(getRoomTemp() <= getTargetTemp() && !isSystemOn) {
          turn(ON);
        } else if(getRoomTemp() > getTargetTemp() && isSystemOn) {
          turn(OFF);
        }
        break;
      case COOLING:
        if(getRoomTemp() >= getTargetTemp() && !isSystemOn) {
          turn(ON);
        } else if(getRoomTemp() < getTargetTemp() && isSystemOn) {
          turn(OFF);
        }
        break;
      default:
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
        }
        HEAT(on_or_off);
        break;
      case COOLING:
        if(IS_ON(HEAT_PIN)) {
          HEAT(OFF);
        }
        COOL(on_or_off);
        break;
      default:
        ;
    }
    lastTurnTime = millis();
  }
}

// Alternative digitalRead function
// Should this really be part of this class?
int TempControl::digitalReadAlt(int pin) const {
  return ((*portOutputRegister(digitalPinToPort(pin)) & digitalPinToBitMask(pin)) ? HIGH : LOW);
}

// Reads the temperatures from the thermistor circuit in the proper units
void TempControl::captureRoomTemp() {
  long temp = READTEMP;
  temp = (TEMP_RATE * temp + TEMP_OFFSET) / TEMP_DIV_FACTOR;
//  temp = ((3 * (long)roomTemp) + temp) / 4; // filter
  roomTemp = (unit == CELCIUS) ?  (int)temp : convertTemp((int)temp, FAHRENHEIT);
}

int TempControl::convertTemp(int tempVal, byte convertTo) {
  long temp = tempVal;
  switch(convertTo) {
    case CELCIUS:
      temp = ((temp - (32 * TEMP_MULTIPLE)) * 5) / 9;
      break;
    case FAHRENHEIT:
      temp = ((9 * temp) / 5) + (32 * TEMP_MULTIPLE); 
      break;
    default:
      ;
  }
  return (int)temp;
}
