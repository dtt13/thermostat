
#ifndef SCREEN_CONTROL_H_
#define SCREEN_CONTROL_H_

#include "TempControl.h"
#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_RA8875.h>

// Pin out
#define RA8875_INT     3
#define RA8875_CS      10
#define RA8875_RESET   9

// Screens views
enum views {STARTUP, THERMOSTAT, SETTINGS}; 

// Touchscreen debouncing
#define TOUCH_DELAY  100 // in milliseconds

//Touchscreen calibration
#define MATRIX_AN      -178752
#define MATRIX_BN      4608
#define MATRIX_CN      6970800
#define MATRIX_DN      -6758
#define MATRIX_EN      -120663
#define MATRIX_FN      17941435
#define MATRIX_DIVIDER  -344035// cannot be zero

#define DEGREE_SYM        0xb0

// Thermostat macros
#define THERMO_UPDATE_DELAY  250 // milliseconds
#define TEMP_PRESS_DELAY     500 // milliseconds 
//// Layers
////enum layers {LAYER1, LAYER2};

// Buttons
typedef struct Button {
  uint16_t x,y,width,height;
} button_t;

class ScreenControl {
  public:
    ScreenControl(TempControl *tc);
    bool init();
    void processTouch();
    void setTouchFlag();

  private:
    Adafruit_RA8875 *tft;// = Adafruit_RA8875(RA8875_CS, RA8875_RESET);
    TempControl *tc;
    uint8_t currentView;
    uint32_t lastTouchCheck, lastThermoUpdate, lastScreenPress;
    uint16_t tx, ty;
    tsMatrix_t cal_matrix;
    bool isPressed, wasPressed, touchFlag;
    button_t tempUpButton, tempDownButton, settingsButton; // thermostat view
    button_t unitsButton, modeButton, backButton; // settings view
    
    // used for display
    void switchView(int view);
    void drawBackground();
    void drawView(int view);
    void drawThermostatViewButtons();
    void drawApp();
    void updateTemps();
    void drawSettingsViewButtons();

    // used for touch
    void processThermostatTouch();
    void processSettingsTouch();
    bool isTouchDown();
    bool isTouchUp();
    
    // using buttons
    void createButton(button_t *button, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
    void displayButton(button_t *button);
    bool isTouched(button_t *button);
    
    // calibrating touch
    void setCalibrationMatrix(tsMatrix_t *matrixPtr);
    void calibrateTSPoint(tsPoint_t *displayPtr, tsPoint_t *screenPtr, tsMatrix_t *matrixPtr);
};

#endif // SCREEN_CONTROL_H_
