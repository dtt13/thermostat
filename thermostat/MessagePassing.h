

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

// Read buffers
#define COMMAND_BUFFER_SIZE    1024

// how long before an image is done streaming
#define STREAM_DELAY           1000 // milliseconds

#define MAX_FAIL_COUNT         3

extern bool isStreaming;

// main processing function
void processCommands(TempControl *tc, ScreenControl *sc);
int readPacket();

// helper functions for interpreting TempControl
String modeToString(TempControl *tc);
String unitToString(TempControl *tc);
String isOnToString(TempControl *tc);

// helper function for unpacking binary data
uint16_t unpackNumber(char *bytes, int start, int len);

#endif // MESSAGE_PASSING_H_
