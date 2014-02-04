#include <Bridge.h>
//#include <Serial1.h>

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_RA8875.h>
#include <TimerOne.h>

#include <errno.h>

// RA8875 library only supports hardware SPI at this time
// Connect SCLK to UNO Digital #13 (Hardware SPI clock)
// Connect MISO to UNO Digital #12 (Hardware SPI MISO)
// Connect MOSI to UNO Digital #11 (Hardware SPI MOSI)
#define RA8875_INT 3
#define RA8875_CS 10
#define RA8875_RESET 9

Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RESET);

// Initial temperature setting and offset... in Fahrenheit
#define INIT_TEMP 72
#define INIT_TEMP_OFFSET 1

#define TIMER1_PERIOD 1000000  // Update the temp every second
#define CALC_MULTIPLE 1000UL  // Improves calculations by preventing rounding error for a couple of extra digits
#define TEMP_SHIFT ((1 << 5) - 1)  // Since there are only 10 bits of AI, gain extra precision by moving the bits over to fill a 16 bit number
#define TEMP_MULTIPLE 100  // Keep things to hundreds of degrees in the code
#define COMP_RESISTOR 10000
#define BASE_RESISTANCE 10000  // Thermistor parameter
#define RES_THERM_NOM 25.0  // Thermistor parameter
#define THERM_B 3950  // Thermistor parameter
#define KELVIN_OFFSET 273.15

#define C2F(x)  (((9 * x) / 5) + (32 * TEMP_MULTIPLE)) 
#define F2C(x) ( (((x - (32 * TEMP_MULTIPLE)) * 5) / 9))

// Pin definitions
#define TEMP_PIN A4
#define HEAT_PIN 4
#define COOL_PIN 5
#define FAN_PIN 6

// Quick MACROs for turning on and off the the heater, A/C, and fan
#define HEAT(x) digitalWrite(HEAT_PIN, x)
#define COOL(x) {digitalWrite(COOL_PIN, x); FAN(ON);}
#define FAN(x) digitalWrite(FAN_PIN, x || fan_override)
#define ON LOW
#define OFF HIGH
#define IS_ON(x) (digitalReadAlt(x) == ON)
// This reads the value from a digital port set to write without clearing the port.  The "protected" version commented below does not work.
//#define digitalReadAlt(pin) ((*portOutputRegister(digitalPinToPort(pin)) & digitalPinToBitMask(pin)) ? HIGH : LOW)
//#define digitalReadAlt(pin) (digitalPinToPort(pin) == NOT_A_PIN) ? LOW : ((*portOutputRegister(digitalPinToPort(pin)) & digitalPinToBitMask(pin)) ? HIGH : LOW)
#define READTEMP analogRead(TEMP_PIN) * TEMP_SHIFT
//#define CHANGE_UNITS(new_units) C_or_F = new_units
#define OFFSET(x) output_temp(temp_output(temp_ref_ui) + x)

// Mode definitions
enum modes {HEATING, COOLING};

enum operation {TEMP, SET};

enum temperature_units {FAHRENHEIT, CELCIUS};

enum screen_modes {THERMOSTAT_GUI, WEATHER_SCREEN};

uint32_t temp_ui = 0, temp_ref_ui = 0;
uint32_t last_time_ui = 0, time_delay_ui = 60000;
int32_t offset_heating = 0, offset_cooling = 0; 
uint8_t touch_flag = 0, read_flag = 0, mode = HEATING, fan_override = OFF, C_or_F = FAHRENHEIT, screen_mode = THERMOSTAT_GUI;

#define TEXT_HEIGHT 16
#define TEXT_WIDTH 8
#define TEXT_SCALE 4

#define BCKGRND 0x0008
#define UP_ARROW RA8875_RED
#define DN_ARROW RA8875_BLUE
#define TXT_CLR 0xC618

#define TEMP_TRI_HEIGHT 80
#define TEMP_TRI_WIDTH 50
#define TEMP_TRI_BORDER 25


// ISR to set a flag for the main loop based on a timer interrupt interval to read the temperature.
void temp_read(void) {
  read_flag = 1;
}

// Display and touch driver interrupt
void touch_isr(void) {
  touch_flag += 5;
}

void set_ref(uint32_t new_ref) {
  temp_ref_ui = output_temp(new_ref);
}

int digitalReadAlt(int pin) {
  return ((*portOutputRegister(digitalPinToPort(pin)) & digitalPinToBitMask(pin)) ? HIGH : LOW);
}

// Calculate the temp based on the analogRead() value.  This is currently in hundredths of degrees C.
uint32_t input_to_temp(uint32_t a_input) {
  return lround((1 /  \
    (log((float)((COMP_RESISTOR * CALC_MULTIPLE) / \
      (((1023 * CALC_MULTIPLE * TEMP_SHIFT) / a_input) - 1 * CALC_MULTIPLE)) / \
    ((float)BASE_RESISTANCE)) / THERM_B + \
    (1 / (RES_THERM_NOM + KELVIN_OFFSET))) - KELVIN_OFFSET) * TEMP_MULTIPLE);
}

// Calculate the machine value for the given temperature.  This is the opposite for the above function.
uint32_t temp_to_input(uint32_t the_temp) {
  return lround((1023 * CALC_MULTIPLE * TEMP_SHIFT) / \
          ((COMP_RESISTOR) / \ 
            (BASE_RESISTANCE * exp(THERM_B * \ 
              (((1 * TEMP_MULTIPLE) / (the_temp + KELVIN_OFFSET * TEMP_MULTIPLE)) - \ 
              ((1) / (RES_THERM_NOM + KELVIN_OFFSET))))) + \
            (1)) / CALC_MULTIPLE);
}

// Generate an appropriate integer for the temperature in the preferred units.
uint8_t temp_output(uint32_t t_conv) {
  return (C_or_F == FAHRENHEIT) ? lround((float)C2F(input_to_temp(t_conv))/TEMP_MULTIPLE) : lround((float)input_to_temp(t_conv)/TEMP_MULTIPLE);
}

// Converts a temperature integer into raw counts
uint32_t output_temp(uint32_t t_conv) {
  return (C_or_F == FAHRENHEIT) ? temp_to_input(F2C(t_conv * TEMP_MULTIPLE)) : temp_to_input(t_conv * TEMP_MULTIPLE);
}

// Turn on and off the heating or cooling and make sure the A/C and Heater are not on at the same time!
void turn(uint8_t on_or_off) {
  if (((millis() - last_time_ui) < time_delay_ui) && (on_or_off == ON))
    return;
  switch (mode) {
    case HEATING:
//      if (IS_ON(COOL_PIN)) {
//        // Raise a warning because if shouldn't happen.
//        COOL(OFF);
//      } 
      HEAT(on_or_off);
      break;
    case COOLING:
//      if (IS_ON(HEAT_PIN)) {
//        // Raise a warning because if shouldn't happen.
//        HEAT(OFF);
//      } 
      COOL(on_or_off);
      break;
  }
    
  last_time_ui = millis();
    
  // Add some logging here.  It should send it to the YUN Arm chip for logging in the future
}

// Switch the mode and make sure the old mode element is off.
void switch_mode(int new_mode) {
//  if ((new_mode != HEATING) && (new_mode != COOLING)) {
//    // Raise a warning
//    return;
//  }
  if (new_mode != mode) {
    // Log the change of mode
    turn(OFF);  
    mode = new_mode;
  }

  return;
}

// Process commands sent via the Serial port to control the system.
void process_commands() {
  int error = 0;
  char ret[8];

  while (Serial1.available() > 2) {
    memset(ret, 0x00, sizeof(ret));
    
    Serial1.readBytes(ret, 2);

    switch (ret[0]) {
      case 'T' :
        if (ret[1] == 0x0A)
          Serial1.println("T" +String(temp_output(temp_ui)));
        else
          error = 1;
        break;
      case 'R' :
        if (ret[1] == 0x0A) {
          Serial1.println("R" + String(temp_output(temp_ref_ui)));
          break;
        }
        else if (isdigit(ret[1])) {
          byte num = Serial1.readBytesUntil(0x0A, ret+2, 6);
          uint8_t new_temp = strtol(&ret[1], NULL, 10);
          if ((new_temp != 0) && (errno != ERANGE))
            set_ref(new_temp);
        }
        else if (ret[1] == '+') {
          ret[2] = Serial1.read();
          set_ref(temp_output(temp_ref_ui) + 1);
        }
        else if (ret[1] == '-') {
          ret[2] = Serial1.read();
          set_ref(temp_output(temp_ref_ui) + 1);
        }
        else 
          break;
        Serial1.println("R" + String(temp_output(temp_ref_ui)));
        break;
      case 'M' :
        if (ret[1] == 0x0A) {
          Serial1.println("M" + String(mode));
          break;
        }
        else if (isdigit(ret[1])) {
          byte num = Serial1.readBytesUntil(0x0A, ret+2, 14);
          uint8_t new_mode = strtol(&ret[1], NULL, 10);
          if ((new_mode != 0) && (errno != ERANGE) && ((new_mode == 0) || (new_mode == 1)))
            switch_mode(new_mode);
        }
        else
          break;
        Serial1.println("M" + String(mode));
        break;
      case 'F' :
        if (ret[1] == 0x0A) {
          Serial1.println("F" + String(IS_ON(FAN_PIN)));
          break;
        }
        else if (isdigit(ret[1])) {
          byte num = Serial1.readBytesUntil(0x0A, ret+2, sizeof(ret)-2);
          uint8_t fan_time = strtol(&ret[1], NULL, 10);
          if ((fan_time != 0) && (errno != ERANGE) && ((fan_time == 0) || (fan_time == 1))) {
            if (fan_time == 0) {
              fan_override = !fan_override;
            }
          }
//            fan_timer(fan_time);
        }
        else
          break;
        Serial1.println("M" + String(IS_ON(FAN_PIN)));
        break;
      case 'B' :
        uint16_t x_pos, y_pos, x_size, y_size;
        uint32_t num, starttime;
        tft.graphicsMode();
        
        if (screen_mode != WEATHER_SCREEN)
          tft.fillScreen(BCKGRND);

        screen_mode = WEATHER_SCREEN;

        while(Serial1.available() < 16);
        x_pos = ret[1] | (Serial1.read() << 8);
        y_pos = Serial1.read() | (Serial1.read() << 8);
        x_size = Serial1.read() | (Serial1.read() << 8);
        y_size = Serial1.read() | (Serial1.read() << 8);
        num = ((uint32_t)x_size) * ((uint32_t)y_size);
        
//        Serial.println(String(x_pos) + " " + String(y_pos) + " " + String(x_size) + " " + String(y_size) + " " + String(num));

        /* Set active window X */
        tft.writeReg(RA8875_HSAW0, x_pos & 0xFF);                             // horizontal start point
        tft.writeReg(RA8875_HSAW1, (x_pos >> 8));
        tft.writeReg(RA8875_HEAW0, (x_pos + x_size - 1) & 0xFF);              // horizontal end point
        tft.writeReg(RA8875_HEAW1, (x_pos + x_size - 1) >> 8);
        
        /* Set active window Y */
        tft.writeReg(RA8875_VSAW0, y_pos & 0xFF);                             // vertical start point
        tft.writeReg(RA8875_VSAW1, (y_pos >> 8));  
        tft.writeReg(RA8875_VEAW0, (y_pos + y_size - 1) & 0xFF);              // horizontal end point
        tft.writeReg(RA8875_VEAW1, (y_pos + y_size - 1) >> 8);

        tft.setXY(x_pos, y_pos);
        tft.writeCommand(RA8875_MRWC);

        digitalWrite(RA8875_CS, LOW);
        SPI.transfer(RA8875_DATAWRITE);
        starttime = millis();
        while ((num--) && ((millis() - starttime) < 1000)) {
          while ((Serial1.available() < 2) && ((millis() - starttime) < 1000));
          if ((millis() - starttime) >= 1000)
            break;
          starttime = millis();
          Serial1.readBytes(ret, 2); 
          SPI.transfer(ret[1]);
          SPI.transfer(ret[0]);
        }
        digitalWrite(RA8875_CS, HIGH);

        /* Set active window X */
        tft.writeReg(RA8875_HSAW0, 0);                                        // horizontal start point
        tft.writeReg(RA8875_HSAW1, 0);
        tft.writeReg(RA8875_HEAW0, (uint16_t)(tft.width() - 1) & 0xFF);            // horizontal end point
        tft.writeReg(RA8875_HEAW1, (uint16_t)(tft.width ()- 1) >> 8);
        
        /* Set active window Y */
        tft.writeReg(RA8875_VSAW0, 0);                                        // vertical start point
        tft.writeReg(RA8875_VSAW1, 0);  
        tft.writeReg(RA8875_VEAW0, (uint16_t)(tft.height() - 1) & 0xFF);           // horizontal end point
        tft.writeReg(RA8875_VEAW1, (uint16_t)(tft.height() - 1) >> 8);
        
        tft.textMode();
        
        break;
      case 'G' :
        if (ret[1] == 0x0A) {
          if (screen_mode != THERMOSTAT_GUI) {
            screen_mode = THERMOSTAT_GUI;
            draw_buttons();
            update_display();
          }
        }
        else {
          error = 1;
        }
        break;
//      case '/' :
//      case '?' :
//        if (ret[1] == 0x0A) {
//          Serial1.println("Update: mode is " + String(mode) + \
//            ", the current temperature is " + String(temp_output(temp_ui)) + \
//            " and the set point is " + String(temp_output(temp_ref_ui)) + \
//            " HEAT/COOL/FAN_PIN " + String(IS_ON(HEAT_PIN)) + " " + String(IS_ON(COOL_PIN)) + " " + String(IS_ON(FAN_PIN)));
//        }
//        else {
//          error = 1;
//        }
//        break;
//      default :
//        error = 1;
//        break;
    }
    if (error) {
      while (Serial1.available()) {
        Serial1.readBytes(ret, min(Serial1.available(), sizeof(ret)));
      }
      Serial1.println("Error");
    }
  }
  return;
}

//  Kind of debounce touches and process them
void process_touchscreen(void) {
  uint32_t cnt, x, y;
  static uint32_t last_touch;
  uint16_t tempx, tempy;

  if (touch_flag) {
    touch_flag = 0;

    if(tft.touched()) {

      tft.touchRead(&tempx, &tempy);
      if ((millis() - last_touch) < 250) {
        return;
      }
      last_touch = millis();
      if (screen_mode != THERMOSTAT_GUI) {
        screen_mode = THERMOSTAT_GUI;
        draw_buttons();
        update_display();
        return;
      }

      x = ((tempx * ((uint32_t)(tft.width()))) >> 10);
      y = ((tempy * ((uint32_t)(tft.height()))) >> 10);

      if((x > (tft.width() - TEMP_TRI_WIDTH)/2) && (x < (tft.width() + TEMP_TRI_WIDTH)/2) && \
          (y > TEMP_TRI_BORDER) && (y < TEMP_TRI_BORDER + TEMP_TRI_HEIGHT)) {
        temp_ref_ui = output_temp(temp_output(temp_ref_ui) + 1);
      }
      if((x > (tft.width() - TEMP_TRI_WIDTH)/2) && (x < (tft.width() + TEMP_TRI_WIDTH)/2) && \
          (y < (tft.height() - TEMP_TRI_BORDER)) && (y > tft.height() - (tft.height() - (TEMP_TRI_BORDER + TEMP_TRI_HEIGHT)))) {
        temp_ref_ui = output_temp(temp_output(temp_ref_ui) - 1);
      }
      update_display();
    }
  }
  return;
}

// Initialize the touch screen
int init_tft(void) {

  if (!tft.begin(RA8875_480x272)) {
    Serial1.println("RA8875 Not Found!");
    return(-1);
  }

  Serial1.println("Found RA8875");

  tft.displayOn(true);
  tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
  tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
  tft.PWM1out(255);

  pinMode(RA8875_INT, INPUT);
  attachInterrupt(0, touch_isr, FALLING);
  tft.touchEnable(true);

  return(0);
}

// Draw the graphics on the screen
void draw_buttons(void) {

  tft.graphicsMode();
  tft.fillScreen(BCKGRND);

  // Put arrows on the screen
  tft.fillTriangle(tft.width()/2, \
    TEMP_TRI_BORDER, \
    (tft.width() - TEMP_TRI_HEIGHT)/2, \
    TEMP_TRI_BORDER + TEMP_TRI_WIDTH, \
    (tft.width() + TEMP_TRI_HEIGHT)/2, \
    TEMP_TRI_BORDER + TEMP_TRI_WIDTH, \
    UP_ARROW);

  tft.fillTriangle(tft.width()/2, \
    tft.height() - TEMP_TRI_BORDER, \
    (tft.width() - TEMP_TRI_HEIGHT)/2, \
    tft.height() - (TEMP_TRI_BORDER + TEMP_TRI_WIDTH), \
    (tft.width() + TEMP_TRI_HEIGHT)/2, \
    tft.height() - (TEMP_TRI_BORDER + TEMP_TRI_WIDTH), \
    DN_ARROW);  

  return;  
}

void put_text(uint16_t temp, uint16_t x_pos, uint16_t y_pos) {
  char buf[5];
  
//  tft.textMode(); 

  tft.textSetCursor(x_pos, y_pos);
  String tempstr = String(temp_output(temp)) + "  ";
  tempstr.toCharArray(buf, 5);
  tft.textWrite(buf, strlen(buf));
  
  return;
}

// Draw and update the text on the screen
void update_display(void) {
  char buf[5];
  
  if (screen_mode == THERMOSTAT_GUI) {
    tft.textMode(); 
    tft.textColor(TXT_CLR, BCKGRND);
    tft.textEnlarge(TEXT_SCALE - 1);
  
    put_text(temp_ui, tft.width() / 5 - (TEXT_WIDTH * TEXT_SCALE), (tft.height() - (TEXT_HEIGHT * TEXT_SCALE)) / 2);
//    tft.textSetCursor(tft.width() / 5 - (TEXT_WIDTH * TEXT_SCALE), (tft.height() - (TEXT_HEIGHT * TEXT_SCALE)) / 2);
//    String tempstr = String(temp_output(temp_ui)) + "  ";
//    tempstr.toCharArray(buf, 5);
//    tft.textWrite(buf, strlen(buf));

    put_text(temp_ref_ui, tft.width() * 4 / 5 - (TEXT_WIDTH * TEXT_SCALE), (tft.height() - (TEXT_HEIGHT * TEXT_SCALE)) / 2);
//    tempstr = String(temp_output(temp_ref_ui)) + "  ";
//    tempstr.toCharArray(buf, 5);
//    tft.textSetCursor(tft.width() * 4 / 5 - (TEXT_WIDTH * TEXT_SCALE), (tft.height() - (TEXT_HEIGHT * TEXT_SCALE)) / 2);
//    tft.textWrite(buf, strlen(buf));
    
    // Do not know why, but need to do this to keep interrupt from RA8875 alive
    tft.graphicsMode();
  }
  
  return;
}

void process_temperature(void) {
  if (read_flag) {
    read_flag = 0;
    
    // Need to make a better filter here.
    temp_ui = ((temp_ui * 9) + (READTEMP * 1)) / 10;
    
    // this is all in input units, not temperature units, so higher temps are smaller numbers
    if ( ((temp_ui >= OFFSET(offset_heating)) && ((mode == HEATING)) && !IS_ON(HEAT_PIN)) || \
          ((temp_ui <= OFFSET(offset_cooling)) && ((mode == COOLING)) && !IS_ON(COOL_PIN)) ) {
      turn(ON);
    }
    else if ( ((temp_ui <= temp_ref_ui) && (mode == HEATING) && IS_ON(HEAT_PIN)) || \
              ((temp_ui >= temp_ref_ui) && (mode == COOLING) && IS_ON(COOL_PIN)) ) {
      turn(OFF);
    }

    update_display();
  }
  return;  
}

void setup() {
  // put your setup code here, to run once:
  Bridge.begin(); // Need to do something to quiet down the serial port
//  Console.begin();
//  Mailbox.begin();
  Serial.begin(115200);
  Serial1.begin(250000);
  Serial.setTimeout(1);
  Serial1.setTimeout(1);
  
  Serial.println("Initializing output pins.");
  pinMode(HEAT_PIN, OUTPUT); digitalWrite(HEAT_PIN, OFF);
  pinMode(COOL_PIN, OUTPUT); digitalWrite(COOL_PIN, OFF);
  pinMode(FAN_PIN, OUTPUT); digitalWrite(FAN_PIN, OFF);

  temp_ui = READTEMP;
  set_ref(INIT_TEMP);
  offset_heating = -INIT_TEMP_OFFSET;
  offset_cooling = INIT_TEMP_OFFSET;
  Serial.println("Initializing temp reading and references " + String(temp_output(temp_ui)));

//  Serial.println("Initializing timer");  
  Timer1.initialize(TIMER1_PERIOD);
  Timer1.attachInterrupt(temp_read, TIMER1_PERIOD);

  Serial.println("Initialize TFT");
  if (init_tft()) {
    while(1);
  }
  
  Serial.println("Initialize Screen");
  draw_buttons();

}

void loop() {
  // put your main code here, to run repeatedly:
  process_commands();

  process_touchscreen();

  process_temperature();

}

