
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
  lastTouchCheck, lastScreenUpdate, lastScreenPress = 0;
  isPressed, wasPressed, touchFlag = false;
//  touchFlag = true;
  setCalibrationMatrix(&cal_matrix);
  createButton(&tempUpButton, 385, 41, 70, 60);
  createButton(&tempDownButton, 385, 191, 70, 60);
  createButton(&settingsButton, 30, 41, 70, 40);
  createButton(&fanButton, 30, 211, 70, 40);
  createButton(&unitsButton, 200, 110, 70, 40);
  createButton(&modeButton, 200, 185, 70, 40);
  createButton(&backButton, 385, 210, 70, 40);
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
  tft->enableTwoLayers(true);
  tft->enableTransparentMode(true, TRANSPARENT_COLOR);
  tft->layerMode(1);
  tft->fillScreen(BLACK);
  drawBackground();
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
//    Serial.print("Touch: ");
    tft->touchRead(&tx, &ty);
    raw.x = tx;
    raw.y = ty;
    calibrateTSPoint(&calibrated, &raw, &cal_matrix);
    tx = calibrated.x;
    ty = calibrated.y;
//    Serial.println(String(tx) + ", " + String(ty));
    lastTouchCheck = millis();
//    touchFlag = false;
  }
  // update screen
  if(millis() - lastScreenUpdate > SCREEN_UPDATE_DELAY) {
    updateHeader();
    if(currentView == THERMOSTAT) {
      updateTemps();
    }
    lastScreenUpdate = millis();
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

void ScreenControl::layerMode(uint8_t layer) {
  tft->layerMode(layer);
}

void ScreenControl::writeText(uint16_t x, uint16_t y, uint16_t fColor, uint16_t bColor, uint8_t fontSize, char *text) {
  tft->textMode();
  tft->textSetCursor(x, y);
  tft->textColor(fColor, bColor);
  tft->textEnlarge(fontSize);
  tft->textWrite(text);
}

void ScreenControl::writeText(uint16_t x, uint16_t y, uint16_t color, uint8_t fontSize, char *text) {
  tft->textMode();
  tft->textSetCursor(x, y);
  tft->textTransparent(color);
  tft->textEnlarge(fontSize);
  tft->textWrite(text);
}

void ScreenControl::drawImage(uint16_t *data, int len, uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
  tft->graphicsMode();
  uint16_t currentX, currentY;
  for(currentY = y; currentY < y + height; currentY++) {
    for(currentX = x; currentX < x + width; currentX++) {
      Serial.print("x, y, data: " + String(currentX) + ", " + String(currentY) + ", ");
      Serial.println(data[(currentY - y) * width + (currentX - x)], HEX);
      uint16_t pixel = data[(currentY - y) * width + (currentX - x)];
      pixel = ((pixel & 0xFF00) >> 8) | ((pixel & 0x00FF) << 8);
      tft->drawPixel(currentX, currentY, pixel);
    }
  } 
}

void ScreenControl::hideApp(bool hide) {
  uint16_t color;
  if(hide) {
    color = PRIMARY_BLUE;
  } else {
    color = TRANSPARENT_COLOR;
  }
  tft->fillRect(120, 40, 240, 212, color);
}

// changes the view and draws the display for that view
void ScreenControl::switchView(int view) {
  currentView = view;
  drawView(view);
}

// draws a simple background theme for all views
void ScreenControl::drawBackground() {
  tft->layerMode(2);
  tft->fillScreen(GRAY);
  tft->fillRect(10, 30, 460, 232, PRIMARY_BLUE);
  tft->fillRect(0, 0, 480, 20, BLACK);
  tft->layerMode(1);
}

// outputs the specified view to the display
void ScreenControl::drawView(int view) {
  char text[20]; 
  tft->fillScreen(TRANSPARENT_COLOR);
  hideApp(true);
  switch(view) {
    case STARTUP:
      strcpy(text, "Welcome to Roost!");
      writeText(100, 125, WHITE, 1, text);
      break;
    case THERMOSTAT:
      drawThermostatViewButtons();
      updateTemps();
//      drawApp();
      break;
    case SETTINGS:
      // header
      strcpy(text, "Settings");
      writeText(20, 35, WHITE, 1, text);
      // units
      strcpy(text, "units:");
      writeText(40, 110, WHITE, 1, text);
      // mode
      strcpy(text, "mode:");
      writeText(40, 185, WHITE, 1, text);
      drawSettingsViewButtons();
      break;
    default:
      switchView(STARTUP);
  }
}

// draws the arrow temperature control buttons on the screen
void ScreenControl::drawThermostatViewButtons() {
  char text[12];
  int centerX = 420;
  int centerY = 146;
  int separation = 50;
  displayButton(&tempUpButton);
  displayButton(&tempDownButton);
  displayButton(&settingsButton);
  displayButton(&fanButton);
  // temperature control buttons
  tft->fillTriangle(centerX - 30, centerY - separation, centerX + 30, centerY - separation, centerX, centerY - (separation + 50), PRIMARY_RED); // heat up
  tft->fillTriangle(centerX - 30, centerY + separation, centerX + 30, centerY + separation, centerX, centerY + (separation + 50), SECONDARY_BLUE); // cool down
  // settings button
  strcpy(text, "Settings");
  writeText(35, centerY - (separation + 45), BLACK, 0, text);
  strcpy(text, "Fan");
  writeText(53, centerY + (separation + 25), BLACK, 0, text);
}

void ScreenControl::drawApp() {
  char text[15] = "App goes here.";
  tft->layerMode(2);
  tft->fillRect(120, 40, 240, 212, BLACK);
  writeText(130, 125, WHITE, 1, text);
  tft->layerMode(1);
}

//
void ScreenControl::updateHeader() {
  char text[4];
  tft->layerMode(2);
  // system on?
  if(tc->isOn()) {
    strcpy(text, "On ");
    writeText(415, 5, GREEN, BLACK, 0, text); 
  } else {
    strcpy(text, "Off");
    writeText(415, 5, PRIMARY_RED, BLACK, 0, text);
  }
  // fan on?
  strcpy(text, "Fan");
  if(tc->isFanOn()) {
    writeText(50, 5, PALE_YELLOW, BLACK, 0, text);
  } else {
    writeText(50, 5, BLACK, BLACK, 0, text);
  }
  tft->layerMode(1);
}


// writes the updated temperatures to the screen
void ScreenControl::updateTemps() {
  char text[4];
  // target temp
  itoa(tc->getTargetTemp(), text, 10);
  writeText(385, 110, WHITE, PRIMARY_BLUE, 6, text);
  // room temp
  itoa(tc->getRoomTemp(), text, 10);
  writeText(30, 110, WHITE, PRIMARY_BLUE, 6, text);
}

// processes touch events on the thermostat view
void ScreenControl::processThermostatTouch() {
  if(isTouchDown() && isTouched(&fanButton)) {
    tc->switchFan();
  } else if(isTouchDown() || (isPressed && (millis() - lastScreenPress > TEMP_PRESS_DELAY))) {
    if(isTouched(&tempUpButton)) { // heat up
      tc->incrementTargetTemp();
      hideApp(true);
    } else if(isTouched(&tempDownButton)) { // cool down
      tc->decrementTargetTemp();
      hideApp(false);
    }
    lastScreenPress = millis();
  } else if(isTouchUp()) {
    if(isTouched(&settingsButton)) {
      switchView(SETTINGS);
    }
  }
}

void ScreenControl::drawSettingsViewButtons() {
  char text[8];
  displayButton(&unitsButton);
  displayButton(&modeButton);
  displayButton(&backButton);
  // units
  if(tc->getUnit() == FAHRENHEIT) {
    strcpy(text, " F");
  } else {
    strcpy(text, " C");
  }
  text[0] = DEGREE_SYM;
  writeText(225, 120, BLACK, 0, text);
  // mode
  if(tc->getMode() == HEATING) {
    strcpy(text, "Heating");
  } else {
    strcpy(text, "Cooling");
  }
  writeText(207, 195, BLACK, 0, text);
  // back
  strcpy(text, "Back");
  writeText(405, 220, BLACK, 0, text);
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
  tft->fillRoundRect(button->x, button->y, button->width, button->height, 5, GRAY);
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
