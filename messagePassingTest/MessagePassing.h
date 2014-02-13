

#ifndef MESSAGE_PASSING_H_
#define MESSAGE_PASSING_H_

#include <Bridge.h>
#include <string.h>

#define SET_TARGET_TEMP        'S'
#define GET_TARGET_TEMP        'G'
#define GET_ROOM_TEMP          'R'
#define INCREMENT_TARGET_TEMP  'I'
#define DECREMENT_TARGET_TEMP  'D'

void processCommands();

#endif // MESSAGE_PASSING_H_
