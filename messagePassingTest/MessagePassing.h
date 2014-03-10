

#ifndef MESSAGE_PASSING_H_
#define MESSAGE_PASSING_H_

#include <Bridge.h>
#include <string.h>
#include "TempControl.h"

// Command codes
#define GET_ROOM_TEMP          	'R'
#define GET_TARGET_TEMP        	'G'
#define GET_MODE				'H'
#define GET_UNIT				'J'
#define IS_ON					'O'
#define SET_TARGET_TEMP        	'S'
#define INCREMENT_TARGET_TEMP  	'I'
#define DECREMENT_TARGET_TEMP  	'D'
#define SWITCH_MODE				'M'
#define SWITCH_UNIT				'U'
#define STREAM_IMAGE            'P'

// Image command codes
#define STREAM_INIT         'i'
#define STREAM_SEND         's'
#define STREAM_FIN          'f'

// Read buffers
#define COMMAND_BUFFER_SIZE    16
#define IMAGE_BUFFER_SIZE      512
#define MAX_FAIL_COUNT         3

void processCommands(TempControl *tc);
String modeToString(TempControl *tc);
String unitToString(TempControl *tc);
String isOnToString(TempControl *tc);
int interpretNumber(char *number, int len); // TODO to be removed
uint16_t unpackNumber(char *bytes, int start, int len);
void receiveImage(uint16_t xpos, uint16_t ypos, uint16_t width, uint16_t height);
void imageInitResponse();
uint16_t imageSendResponse(char *image, int len, uint16_t pixelCount);
void writeResponse(char *response, int len);
uint16_t imageToScreen(char *image, int len, uint16_t pixelCount);

#endif // MESSAGE_PASSING_H_
