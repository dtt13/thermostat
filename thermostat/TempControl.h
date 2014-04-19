#ifndef TEMP_CONTROL_H_
#define TEMP_CONTROL_H_

#include <Arduino.h>
#include <TimerOne.h>

// Heating and cooling functions
#define HEAT(x)         digitalWrite(HEAT_PIN, x)
#define COOL(x)         {digitalWrite(COOL_PIN, x); FAN(ON);}
#define FAN(x)          digitalWrite(FAN_PIN, x && fanOnOverride)
#define ON LOW
#define OFF HIGH
#define IS_ON(x)        (digitalReadAlt(x) == ON)
#define ON_TIME_DELAY        60000 // wait 1 minute before turning system back on
#define TEMP_UPDATE_DELAY    1000 // update the room temperature reading every second

#define READTEMP analogRead(TEMP_PIN) //* TEMP_SHIFT
#define OFFSET(x) output_temp(temp_output(temp_ref_ui) + x)

// Quick temperature conversions
#define C2F(x)      (((9 * x) / 5) + (32 * TEMP_MULTIPLE)) 
#define F2C(x)      ((((x - (32 * TEMP_MULTIPLE)) * 5) / 9))

// Conversion calculations
#define CALC_MULTIPLE    1000UL          // Improves calculations by preventing rounding error for a couple of extra digits
#define TEMP_SHIFT       ((1 << 5) - 1)  // Since there are only 10 bits of AI, gain extra precision by movingg the bits over
#define TEMP_MULTIPLE    100             // Keep things to hundreds of degrees in the code
#define COMP_RESISTOR    10000
#define BASE_RESISTANCE  10000           // Thermistor parameter
#define RES_THERM_NOM    25.0            // Thermistor parameter
#define THERM_B          3950            // Thermistor parameter
#define KELVIN_OFFSET    273.15
#define CELCIUS_OFFSET   11.11

// Pin definitions
#define TEMP_PIN 	A4
#define HEAT_PIN 	4
#define COOL_PIN 	5
#define FAN_PIN 	6

// Modes of operation
enum modes {HEATING, COOLING};
enum temperature_units {FAHRENHEIT, CELCIUS};

// Defaults upon first start up
#define DEFAULT_TARGET_TEMP     75
#define DEFAULT_ROOM_TEMP       65
#define DEFAULT_MODE            HEATING
#define DEFAULT_UNIT            FAHRENHEIT

class TempControl {
  public:
    TempControl();
    // Get methods
    int getRoomTemp();
    int getTargetTemp();
    uint8_t getMode();
    uint8_t getUnit();
    bool isOn();
    bool isFanOn();
    // Control methods
    void setTargetTemp(int targetTemp);
    void incrementTargetTemp();
    void decrementTargetTemp();
    void switchMode();
    void switchFan();
    void switchUnit();
    // Updating methods
//    void processTemperature();

  private:
    int roomTemp, targetTemp;
    bool isSystemOn;
    uint8_t mode, unit, fanOnOverride;
    uint32_t lastTurnTime, lastTempUpdateTime;
    void turn(uint8_t on_or_off);
    int digitalReadAlt(int pin) const;
//    int convertRawTemp(int raw);
};

#endif // TEMP_CONTROL_H_
