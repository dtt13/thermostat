
#include "ScreenControl.h"
#include "ColorScheme.h"

// constructor for the ScreenControl class
ScreenControl::ScreenControl() {
  // initialize the screen object
  tft = new Adafruit_RA8875(RA8875_CS, RA8875_RESET);
  // display vars
  currentView = STARTUP;
  // touch vars
  lastTouchCheck = 0;
  isPressed = false;
  wasPressed = false;
  setCalibrationMatrix(&cal_matrix);
//  currentLayer = LAYER1;
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
  tft->fillScreen(RA8875_BLACK);
  
  // setup touch enable
  pinMode(RA8875_INT, INPUT);
  digitalWrite(RA8875_INT, HIGH);
  tft->touchEnable(true);
  
  drawStartupView();
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

// changes the view and draws the display for startup view
void ScreenControl::switchToStartupView() {
  if(currentView != STARTUP) {
    currentView = STARTUP;
    drawStartupView();
  }
}

// changes the view and draws the display for loading view
void ScreenControl::switchToLoadingView() {
  if(currentView != LOADING) {
    currentView = LOADING;
    drawLoadingView();
  }
}

// changes the view and draws the display for thermostat view
void ScreenControl::switchToThermostatView() {
  if(currentView != THERMOSTAT) {
    currentView = THERMOSTAT;
    drawThermostatView();
  }
}

// draws a simple background theme for all views
void ScreenControl::drawBackground() {
  tft->fillScreen(GRAY);
  tft->fillRect(30, 30, 420, 212, PRIMARY_BLUE);
}

// outputs the startup view to the display
void ScreenControl::drawStartupView() {
  char startupText[20] = "Welcome to Thermos!";
  drawBackground();
  tft->textMode();
  tft->textSetCursor(90, 120);
  tft->textTransparent(WHITE);
  tft->textEnlarge(1);
  tft->textWrite(startupText);
}

// outputs the loading view to the display
void ScreenControl::drawLoadingView() {
  char loadingText[11] = "Loading...";
  drawBackground();
  tft->textMode();
  tft->textSetCursor(160, 120);
  tft->textTransparent(WHITE);
  tft->textEnlarge(1);
  tft->textWrite(loadingText);
}

// outputs the thermostat view to the display
void ScreenControl::drawThermostatView() {
  drawBackground();
  int centerX = 380;
  int centerY = 136;
  tft->fillTriangle(centerX - 30, centerY - 20, centerX + 30, centerY - 20, centerX, centerY - 70, PRIMARY_RED); // heat up
  tft->fillTriangle(centerX - 30, centerY + 20, centerX + 30, centerY + 20, centerX, centerY + 70, SECONDARY_BLUE); // cool down
  tft->textMode();
  tft->textTransparent(WHITE);
  tft->textEnlarge(6);
  // target temp
  tft->textSetCursor(70, 100);
  tft->textWrite("65");
  // room temp
  tft->textSetCursor(200, 100);
  tft->textWrite("70");
}

// processes touch events on the startup view
void ScreenControl::processStartupTouch() {
  if(isTouchUp()) {
    switchToLoadingView();
  }
}

// processes touch events on the loading view
void ScreenControl::processLoadingTouch() {
  if(isTouchUp()) {
    switchToThermostatView();
  }
}

// processes touch events on the thermostat view
void ScreenControl::processThermostatTouch() {
  if(isTouchUp()) {
    switchToStartupView();
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

