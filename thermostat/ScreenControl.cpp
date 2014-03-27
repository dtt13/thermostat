
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

// sets the touch flag when a touchscreen interrupt is received
void ScreenControl::setTouchFlag() {
  touchFlag = true;
}

// changes the view and draws the display for that view
void ScreenControl::switchView(int view) {
  if(currentView != view) {
    currentView = view;
    drawView(view);
  }
}

// draws a simple background theme for all views
void ScreenControl::drawBackground() {
  tft->fillScreen(GRAY);
  tft->fillRect(30, 30, 420, 212, PRIMARY_BLUE);
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
    case LOADING:
      strcpy(text, "Loading...");
      drawBackground();
      tft->textMode();
      tft->textSetCursor(160, 120);
      tft->textTransparent(WHITE);
      tft->textEnlarge(1);
      tft->textWrite(text);
      break;
    case THERMOSTAT:
      drawBackground();
      drawTempButtons();
      updateTemps();
      break;
    default:
      switchView(STARTUP);
  }
}

// draws the arrow temperature control buttons on the screen
void ScreenControl::drawTempButtons() {
  int centerX = 400;
  int centerY = 136;
  int separation = 50;
  tft->fillTriangle(centerX - 30, centerY - separation, centerX + 30, centerY - separation, centerX, centerY - (separation + 50), PRIMARY_RED); // heat up
  tft->fillTriangle(centerX - 30, centerY + separation, centerX + 30, centerY + separation, centerX, centerY + (separation + 50), SECONDARY_BLUE); // cool down
}

// writes the updated temperatures to the screen
void ScreenControl::updateTemps() {
  char temp[4];
  tft->textMode();
  tft->textColor(WHITE, PRIMARY_BLUE);
  tft->textEnlarge(6);
  // target temp
  itoa(tc->getTargetTemp(), temp, 10);
  tft->textSetCursor(365, 100);
  tft->textWrite(temp);
  // room temp
  itoa(tc->getRoomTemp(), temp, 10);
  tft->textSetCursor(200, 100);
  tft->textWrite(temp); // TODO may be better way of doing this
  lastThermoUpdate = millis();
}

// processes touch events on the startup view
void ScreenControl::processStartupTouch() {
  if(isTouchUp()) {
    switchView(LOADING);
  }
}

// processes touch events on the loading view
void ScreenControl::processLoadingTouch() {
  if(isTouchUp()) {
    switchView(THERMOSTAT);
  }
}

// processes touch events on the thermostat view
void ScreenControl::processThermostatTouch() {
  if(millis() - lastThermoUpdate > THERMO_UPDATE_DELAY) {
    updateTemps();
  }
  if(isTouchDown() || (isPressed && (millis() - lastScreenPress > TEMP_PRESS_DELAY))) {
    if(340 < tx && tx < 450 && 30 < ty && ty < 100) { // heat up
      tc->incrementTargetTemp();
    } else if(340 < tx && tx < 450 && 172 < ty && ty < 242) { // cool down
      tc->decrementTargetTemp();
    }
    lastScreenPress = millis();
  } else if(isTouchUp() && tx < 200) {
    switchView(STARTUP);
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
