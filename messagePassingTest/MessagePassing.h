

#ifndef MESSAGE_PASSING_H_
#define MESSAGE_PASSING_H_

#include <Bridge.h>
#include <string.h>
#include "TempControl.h"

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

void processCommands(TempControl *tc);
String modeToString(TempControl *tc);
String unitToString(TempControl *tc);
String isOnToString(TempControl *tc);
int interpretNumber(char *number, int len);
uint16_t unpackNumber(char *bytes, int start, int len);

#endif // MESSAGE_PASSING_H_
