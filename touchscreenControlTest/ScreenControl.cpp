
#include "ScreenControl.h"
#include "ColorScheme.h"

//// constructor for the ScreenControl class
ScreenControl::ScreenControl() {
  tft = new Adafruit_RA8875(RA8875_CS, RA8875_RESET);
  currentView = STARTUP;
  lastTouch = 0;
//  currentLayer = LAYER1;
}

//
bool ScreenControl::init() {
  // connect to the screen
  if (!tft->begin(RA8875_480x272)) {
    Serial.println("RA8875 Not Found!");
    return false;
  }
  Serial.println("Found RA8875");
  
  // setup display
  tft->displayOn(true);
  tft->GPIOX(true);      // Enable TFT - display enable tied to GPIOX
  tft->PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
  tft->PWM1out(255);
  tft->fillScreen(RA8875_BLACK);
  
  // setup touch enable
  pinMode(RA8875_INT, INPUT);
  digitalWrite(RA8875_INT, HIGH);
  tft->touchEnable(true);
  
  drawStartupView();
  return true;
}

void ScreenControl::processTouch() {
  // TODO get the touchscreen coordinates pressed
  uint16_t x = 0;
  uint16_t y = 0;
//  Serial.println("processing touch...");
  if(millis() - lastTouch > TOUCH_DELAY) {
    if(tft->touched()) {
      lastTouch = millis();
      Serial.print("Touch: ");
      tft->touchRead(&x, &y);
      Serial.println(String(x) + ", " + String(y));
  
      switch(currentView) {
        case STARTUP:
          processStartupTouch(x, y);
          break;
        case LOADING:
          processLoadingTouch(x, y);
          break;
        case THERMOSTAT:
          processThermostatTouch(x, y);
          break;
        default:
          // TODO log an error
          ;
      }
      tft->touchRead(&x, &y);
    }
  }
}

void ScreenControl::switchToStartupView() {
  if(currentView != STARTUP) {
    currentView = STARTUP;
    drawStartupView();
  }
}

void ScreenControl::switchToLoadingView() {
  if(currentView != LOADING) {
    currentView = LOADING;
    drawLoadingView();
  }
}

void ScreenControl::switchToThermostatView() {
  if(currentView != THERMOSTAT) {
    currentView = THERMOSTAT;
    drawThermostatView();
  }
}

void ScreenControl::drawBackground() {
  tft->fillScreen(GRAY);
  tft->fillRect(30, 30, 420, 212, PRIMARY_BLUE);
}

void ScreenControl::drawStartupView() {
  char startupText[20] = "Welcome to Thermos!";
  drawBackground();
  tft->textMode();
  tft->textSetCursor(90, 120);
  tft->textTransparent(WHITE);
  tft->textEnlarge(1);
  tft->textWrite(startupText);
}

void ScreenControl::drawLoadingView() {
  char loadingText[11] = "Loading...";
  drawBackground();
  tft->textMode();
  tft->textSetCursor(160, 120);
  tft->textTransparent(WHITE);
  tft->textEnlarge(1);
  tft->textWrite(loadingText);
}

void ScreenControl::drawThermostatView() {
  drawBackground();
  int centerX = 380;
  int centerY = 136;
  tft->fillTriangle(centerX - 30, centerY - 20, centerX + 30, centerY - 20, centerX, centerY - 70, PRIMARY_RED); // heat up
  tft->fillTriangle(centerX - 30, centerY + 20, centerX + 30, centerY + 20, centerX, centerY + 70, SECONDARY_BLUE); // cool down
  tft->textMode();
  tft->textTransparent(WHITE);
  tft->textEnlarge(2);
  // target temp
//  tft->textSetCursor();
//  tft->textWrite();
  // room temp
//  tft->textSetCursor();
//  tft->textWrite();
}

void ScreenControl::processStartupTouch(uint16_t x, uint16_t y) {
  switchToLoadingView();
}


void ScreenControl::processLoadingTouch(uint16_t x, uint16_t y) {
  switchToThermostatView();
}


void ScreenControl::processThermostatTouch(uint16_t x, uint16_t y) {
  switchToStartupView();
}

