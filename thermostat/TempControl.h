#ifndef TEMP_CONTROL_H_
#define TEMP_CONTROL_H_

#include <Arduino.h>
#include <TimerOne.h>

// Heating and cooling functions
#define HEAT(x)         digitalWrite(HEAT_PIN, x)
#define COOL(x)         {digitalWrite(COOL_PIN, x); FAN(x);}
#define FAN(x)          digitalWrite(FAN_PIN, x || fanOnOverride)
#define ON HIGH
#define OFF LOW
#define IS_ON(x)        (digitalReadAlt(x) == ON)
#define ON_TIME_DELAY        10000 // wait 10 seconds before turning system back on
#define TEMP_UPDATE_DELAY    5000 // update the room temperature reading every second

#define READTEMP analogRead(TEMP_PIN) 
#define OFFSET(x) output_temp(temp_output(temp_ref_ui) + x)

#define TEMP_MULTIPLE      100
#define TEMP_RATE          -895
#define TEMP_OFFSET        708510
#define TEMP_DIV_FACTOR    100


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
    void processTemperature();

  private:
    int roomTemp, targetTemp;
    bool isSystemOn;
    uint8_t mode, unit, fanOnOverride;
    uint32_t lastTurnTime, lastTempUpdateTime;
    void coerceTargetTemp();
    void turn(uint8_t on_or_off);
    int digitalReadAlt(int pin) const;
    void captureRoomTemp();
    int convertTemp(int tempVal, byte convertTo);
};

#endif // TEMP_CONTROL_H_
