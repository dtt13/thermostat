
#include "ScreenControl.h"
#include "ColorScheme.h"

//// constructor for the ScreenControl class
ScreenControl::ScreenControl() {
  tft = new Adafruit_RA8875(RA8875_CS, RA8875_RESET);
  currentView = STARTUP;
  lastTouchCheck = 0;
  isPressed = false;
  wasPressed = false;
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
  // update the touchscreen coordinates pressed
  wasPressed = isPressed;
  if((millis() - lastTouchCheck > TOUCH_DELAY) && (isPressed = tft->touched())) {
    Serial.print("Touch: ");
    tft->touchRead(&tx, &ty);
    Serial.println(String(tx) + ", " + String(ty));
    lastTouchCheck = millis();
  }
  // process touch depending on the view
  switch(currentView) {
    case STARTUP:
      processStartupTouch();
      break;
    case LOADING:
      processLoadingTouch();
      break;
    case THERMOSTAT:
      processThermostatTouch();
      break;
    default:
      // TODO log an error
      ;
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

void ScreenControl::processStartupTouch() {
  if(isTouchUp()) {
    switchToLoadingView();
  }
}


void ScreenControl::processLoadingTouch() {
  if(isTouchUp()) {
    switchToThermostatView();
  }
}


void ScreenControl::processThermostatTouch() {
  if(isTouchUp()) {
    switchToStartupView();
  }
}

bool ScreenControl::isTouchDown() {
  return (isPressed && !wasPressed);
}

bool ScreenControl::isTouchUp() {
  return (!isPressed && wasPressed);
}

