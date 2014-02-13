#ifndef TEMP_CONTROL_H_
#define TEMP_CONTROL_H_

#include <Arduino.h>
#include <TimerOne.h>

// Heating and cooling functions
#define HEAT(x)         digitalWrite(HEAT_PIN, x)
#define COOL(x)         {digitalWrite(COOL_PIN, x); FAN(ON);}
#define FAN(x)          digitalWrite(FAN_PIN, x || fanOverride)
#define ON LOW
#define OFF HIGH
#define IS_ON(x)        (digitalReadAlt(x) == ON)
#define ON_TIME_DELAY   60000 // wait 1 minute before turning system back on

// This reads the value from a digital port set to write without clearing the port. The "protected" version commented below does not work.
//#define digitalReadAlt(pin) ((*portOutputRegister(digitalPinToPort(pin)) & digitalPinToBitMask(pin)) ? HIGH : LOW)
//#define digitalReadAlt(pin) (digitalPinToPort(pin) == NOT_A_PIN) ? LOW : ((*portOutputRegister(digitalPinToPort(pin)) & digitalPinToBitMask(pin)) ? HIGH : LOW)
#define READTEMP analogRead(TEMP_PIN) //* TEMP_SHIFT
//#define CHANGE_UNITS(new_units) C_or_F = new_units
#define OFFSET(x) output_temp(temp_output(temp_ref_ui) + x)

// Quick temperature conversions
#define C2F(x)      (((9 * x) / 5) + (32 * TEMP_MULTIPLE)) 
#define F2C(x)      ((((x - (32 * TEMP_MULTIPLE)) * 5) / 9))

// Pin definitions
#define TEMP_PIN 	A4
#define HEAT_PIN 	4
#define COOL_PIN 	5
#define FAN_PIN 	6

// Mode of operation
enum modes {HEATING, COOLING};
enum temperature_units {FAHRENHEIT, CELCIUS};

class TemperatureControl {
  public:
    TemperatureControl(int targetTemp, uint8_t mode);
    int getRoomTemp();
    int getTargetTemp();
    uint8_t getMode();
    void setTargetTemp(int targetTemp);
    void incrementTargetTemp();
    void decrementTargetTemp();
    void switchMode();

  private:
    int roomTemp, currentTemp;
    uint8_t mode, fanOverride;
    uint32_t lastTurnTime;
    void turn(uint8_t on_or_off);
    int digitalReadAlt(int pin) const;
};

#endif // TEMP_CONTROL_H_
