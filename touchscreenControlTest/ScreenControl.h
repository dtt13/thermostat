
#ifndef SCREEN_CONTROL_H_
#define SCREEN_CONTROL_H_

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_RA8875.h>

// Pin out
#define RA8875_INT     3
#define RA8875_CS      10
#define RA8875_RESET   9

// Screens views
enum views {STARTUP, LOADING, THERMOSTAT, WEATHER_TEMP, WEATHER_MAP}; 

// Touchscreen debouncing
#define TOUCH_DELAY  500 // in milliseconds

//// Layers
////enum layers {LAYER1, LAYER2};

////// Button properties
////enum button_shapes {CIRCLE, RECTANGLE, IMAGE};
////
////struct Button {
////  int16_t x;
////  int16_t y;
////  uint16_t color;
////  uint8_t layer;
////  uint8_t shape;
////};
class ScreenControl {
  public:
    ScreenControl();
    bool init();
    void processTouch();
//    void
//    void clearLayer(uint8_t layer);
//    uint8_t getCurrentLayer();
//    void switchCurrentLayer();
//    bool createCircleButton(int16_t xpos, int16_t ypos, int16_t radius, uint16_t color, uint8_t layer);
 
  private:
    Adafruit_RA8875 *tft;// = Adafruit_RA8875(RA8875_CS, RA8875_RESET);
    uint8_t currentView;
    uint32_t lastTouch;
    void switchToStartupView();
    void switchToLoadingView();
    void switchToThermostatView();
    void drawBackground();
    void drawStartupView();
    void drawLoadingView();
    void drawThermostatView();
//    void drawWeatherTempView();
//    void drawWeatherMapView();
    void processStartupTouch(uint16_t x, uint16_t y);
    void processLoadingTouch(uint16_t x, uint16_t y);
    void processThermostatTouch(uint16_t x, uint16_t y);
//    uint8_t currentLayer;
};

#endif // SCREEN_CONTROL_H_
