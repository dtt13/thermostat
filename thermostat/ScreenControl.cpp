
#include "ScreenControl.h"
#include "ColorScheme.h"

// constructor for the ScreenControl class
ScreenControl::ScreenControl(TempControl* tempControl) {
  tc = tempControl;
  // initialize the screen object
  tft = new Adafruit_RA8875(RA8875_CS, RA8875_RESET);
  // display vars
  currentView = STARTUP;
  // touch vars
  lastTouchCheck, lastThermoUpdate, lastScreenPress = 0;
  isPressed, wasPressed, touchFlag = false;
//  touchFlag = true;
  setCalibrationMatrix(&cal_matrix);
  createButton(&tempUpButton, 385, 31, 70, 60);
  createButton(&tempDownButton, 385, 181, 70, 60);
  createButton(&settingsButton, 30, 31, 70, 40);
  createButton(&unitsButton, 200, 100, 70, 40);
  createButton(&modeButton, 200, 175, 70, 40);
  createButton(&backButton, 385, 200, 70, 40);
}

// initializes the touch screen putting the screen in start up
// enables touch feature
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
  tft->fillScreen(BLACK);
  
  // setup touch enable
  tft->touchEnable(true);
  
  drawView(STARTUP);
  return true;
}

// processes any input from the touch screen and stores the touched coordinates in tx, ty
// calls the current view's process touch method
void ScreenControl::processTouch() {
  // update the touchscreen coordinates pressed
  tsPoint_t raw, calibrated;
  wasPressed = isPressed;
  if((millis() - lastTouchCheck > TOUCH_DELAY) && (isPressed = tft->touched())) {
    Serial.print("Touch: ");
    tft->touchRead(&tx, &ty);
    raw.x = tx;
    raw.y = ty;
    calibrateTSPoint(&calibrated, &raw, &cal_matrix);
    tx = calibrated.x;
    ty = calibrated.y;
    Serial.println(String(tx) + ", " + String(ty));
    lastTouchCheck = millis();
//    touchFlag = false;
  }
  // process touch depending on the view
  switch(currentView) {
    case STARTUP:
      delay(3000);
      switchView(THERMOSTAT);
      break;
    case THERMOSTAT:
      processThermostatTouch();
      break;
    case SETTINGS:
      processSettingsTouch();
      break;
    default:
      // TODO log an error
      ;
  }
}

// sets the touch flag when a touchscreen interrupt is received
void ScreenControl::setTouchFlag() {
  touchFlag = true;
}

// changes the view and draws the display for that view
void ScreenControl::switchView(int view) {
  currentView = view;
  drawView(view);
}

// draws a simple background theme for all views
void ScreenControl::drawBackground() {
  tft->fillScreen(GRAY);
  tft->fillRect(10, 10, 460, 252, PRIMARY_BLUE);
}

// outputs the specified view to the display
void ScreenControl::drawView(int view) {
  char text[20]; 
  switch(view) {
    case STARTUP:
      strcpy(text, "Welcome to Thermos!");
      drawBackground();
      tft->textMode();
      tft->textSetCursor(90, 120);
      tft->textTransparent(WHITE);
      tft->textEnlarge(1);
      tft->textWrite(text);
      break;
    case THERMOSTAT:
      drawBackground();
      drawThermostatViewButtons();
      updateTemps();
      drawApp();
      break;
    case SETTINGS:
      drawBackground();
      tft->textMode();
      tft->textTransparent(WHITE);
      // header
      strcpy(text, "Settings");
      tft->textSetCursor(20, 20);
      tft->textEnlarge(1);
      tft->textWrite(text);
      // units
      strcpy(text, "units:");
      tft->textSetCursor(40, 100);
      tft->textWrite(text);
      // mode
      strcpy(text, "mode:");
      tft->textSetCursor(40, 175);
      tft->textWrite(text);
      drawSettingsViewButtons();
      break;
    default:
      switchView(STARTUP);
  }
}

// draws the arrow temperature control buttons on the screen
void ScreenControl::drawThermostatViewButtons() {
  char text[12] = "Settings";
  int centerX = 420;
  int centerY = 136;
  int separation = 50;
  displayButton(&tempUpButton);
  displayButton(&tempDownButton);
  displayButton(&settingsButton);
  // temperature control buttons
  tft->fillTriangle(centerX - 30, centerY - separation, centerX + 30, centerY - separation, centerX, centerY - (separation + 50), PRIMARY_RED); // heat up
  tft->fillTriangle(centerX - 30, centerY + separation, centerX + 30, centerY + separation, centerX, centerY + (separation + 50), SECONDARY_BLUE); // cool down
  // settings button
  tft->textMode();
  tft->textSetCursor(35, centerY - (separation + 45));
  tft->textTransparent(BLACK);
  tft->textEnlarge(0);
  tft->textWrite(text);
}

void ScreenControl::drawApp() {
  char text[15] = "App goes here.";
  tft->fillRect(120, 30, 240, 212, BLACK);
  tft->textMode();
  tft->textSetCursor(130, 115);
  tft->textTransparent(WHITE);
  tft->textEnlarge(1);
  tft->textWrite(text);
}

// writes the updated temperatures to the screen
void ScreenControl::updateTemps() {
  char text[4];
  tft->textMode();
  tft->textColor(WHITE, PRIMARY_BLUE);
  tft->textEnlarge(6);
  // target temp
  itoa(tc->getTargetTemp(), text, 10);
  tft->textSetCursor(385, 100);
  tft->textWrite(text);
  // room temp
  itoa(tc->getRoomTemp(), text, 10);
  tft->textSetCursor(30, 100);
  tft->textWrite(text); // TODO may be better way of doing this
  // system on?
  tft->textEnlarge(0);
  tft->textSetCursor(50, 225);
  if(tc->isOn()) {
    strcpy(text, "On ");
    tft->textColor(GREEN, PRIMARY_BLUE);
  } else {
    strcpy(text, "Off");
    tft->textColor(PRIMARY_RED, PRIMARY_BLUE);
  }
  tft->textWrite(text);
  lastThermoUpdate = millis();
}

// processes touch events on the thermostat view
void ScreenControl::processThermostatTouch() {
  if(millis() - lastThermoUpdate > THERMO_UPDATE_DELAY) {
    updateTemps();
  }
  if(isTouchDown() || (isPressed && (millis() - lastScreenPress > TEMP_PRESS_DELAY))) {
    if(isTouched(&tempUpButton)) { // heat up
      tc->incrementTargetTemp();
    } else if(isTouched(&tempDownButton)) { // cool down
      tc->decrementTargetTemp();
    }
    lastScreenPress = millis();
  } else if(isTouchUp()) {
    if(isTouched(&settingsButton)) {
      switchView(SETTINGS);
      Serial.println("switched to settings");
    }
  }
}

void ScreenControl::drawSettingsViewButtons() {
  char text[8];
  displayButton(&unitsButton);
  displayButton(&modeButton);
  displayButton(&backButton);
  tft->textMode();
  tft->textTransparent(BLACK);
  tft->textEnlarge(0);
  // units
  tft->textSetCursor(225, 110);
  if(tc->getUnit() == FAHRENHEIT) {
    strcpy(text, " F");
  } else {
    strcpy(text, " C");
  }
  text[0] = DEGREE_SYM;
  tft->textWrite(text);
  // mode
  tft->textSetCursor(207, 185);
  if(tc->getMode() == HEATING) {
    strcpy(text, "Heating");
  } else {
    strcpy(text, "Cooling");
  }
  tft->textWrite(text);
  // back
  tft->textSetCursor(405, 210);
  strcpy(text, "Back");
  tft->textWrite(text);
}

void ScreenControl::processSettingsTouch() {
  if(isTouchDown()) {
    if(isTouched(&unitsButton)) {
      tc->switchUnit();
    } else if(isTouched(&modeButton)) {
      tc->switchMode();
    }
    drawSettingsViewButtons();
  } else if(isTouchUp() && isTouched(&backButton)) {
    switchView(THERMOSTAT);
  }
}

// determines whether or not the screen was just pressed
bool ScreenControl::isTouchDown() {
  return (isPressed && !wasPressed);
}

// determines whether or not the screen was just released
bool ScreenControl::isTouchUp() {
  return (!isPressed && wasPressed);
}

void ScreenControl::createButton(button_t *button, uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
  button->x = x;
  button->y = y;
  button->width = width;
  button->height = height;
}

void ScreenControl::displayButton(button_t *button) {
  tft->fillRect(button->x, button->y, button->width, button->height, GRAY);
}

bool ScreenControl::isTouched(button_t *button) {
  return ((button->x - 20) <= tx && tx <= (button->x + button->width + 20)
          && (button->y - 20) <= ty && ty <= (button->y + button->height + 20));
}

// initializes the calibration matrix
void ScreenControl::setCalibrationMatrix(tsMatrix_t *matrixPtr) {
  matrixPtr->An = MATRIX_AN;
  matrixPtr->Bn = MATRIX_BN;
  matrixPtr->Cn = MATRIX_CN;
  matrixPtr->Dn = MATRIX_DN;
  matrixPtr->En = MATRIX_EN;
  matrixPtr->Fn = MATRIX_FN;
  matrixPtr->Divider = MATRIX_DIVIDER;
}

// calibrates the raw touch screen input into valid display coordinates using the calibration matrix
void ScreenControl::calibrateTSPoint(tsPoint_t *displayPtr, tsPoint_t *screenPtr, tsMatrix_t *matrixPtr) {
  displayPtr->x = ((matrixPtr->An * screenPtr->x) + (matrixPtr->Bn * screenPtr->y) + matrixPtr->Cn) / matrixPtr->Divider;
  displayPtr->y = ((matrixPtr->Dn * screenPtr->x) + (matrixPtr->En * screenPtr->y) + matrixPtr->Fn) / matrixPtr->Divider;
}
