

#ifndef MESSAGE_PASSING_H_
#define MESSAGE_PASSING_H_

//#include <Bridge.h>
//#include <string.h>
#include "TempControl.h"
#include "ScreenControl.h"

// Command codes
#define GET_ROOM_TEMP          	'R'
#define GET_TARGET_TEMP        	'G'
#define GET_MODE		'H'
#define GET_UNIT		'J'
#define IS_ON			'O'
#define SET_TARGET_TEMP        	'S'
#define SWITCH_MODE		'M'
#define SWITCH_UNIT		'U'
#define WRITE_TEXT              'W'
#define STREAM_IMAGE            'P'

// Image command codes
#define STREAM_INIT             'i'
#define STREAM_SEND             's'
#define STREAM_FIN              'f'

// Read buffers
#define COMMAND_BUFFER_SIZE    32
#define IMAGE_BUFFER_SIZE      512
#define MAX_FAIL_COUNT         3

// main processing function
void processCommands(TempControl *tc, ScreenControl *sc);

// helper functions for interpreting TempControl
String modeToString(TempControl *tc);
String unitToString(TempControl *tc);
String isOnToString(TempControl *tc);

uint16_t unpackNumber(char *bytes, int start, int len);

// helper functions for sending images
//void receiveImage(uint16_t xpos, uint16_t ypos, uint16_t width, uint16_t height);
void imageInitResponse();
uint32_t imageSendResponse(char *image, int len);
void imageToScreen(char *image, int len);
void writeResponse(char *response, int len);
//uint16_t imageToScreen(char *image, int len, uint16_t pixelCount);
void imageCopy(char *dest, char *src, int len);

// helper functions for debugging
void printData(char *date, int len);

#endif // MESSAGE_PASSING_H_
