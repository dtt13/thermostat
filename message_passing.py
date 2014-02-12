

import io, struct

# Serial1 connection
_SERIAL1 = '/dev/ttyATH0'

# Message commands constants
_SET_TARGET_TEMP = 'S'
_GET_TARGET_TEMP = 'G'
_GET_ROOM_TEMP = 'R'

# Sets the target temperature
def setTargetTemp(newTargetTemp):
	_sendCommand(_SET_TARGET_TEMP, newTargetTemp, False)

# Gets the target temperature
def getTargetTemp():
	result = _sendCommand(_GET_TARGET_TEMP, '', True)
	return result

# Gets the temperature of the room
def getRoomTemp():
	_sendCommand(_GET_ROOM_TEMP, '', True)

# Sends a command to the microcontroller with the message
# expectedResponse should be True if expecting a response
# from the microcontroller, False otherwise
def _sendCommand(command, message, expectedResponse):
	packet = '%s%s' % (command, message)
	output = io.open(_SERIAL1, 'wb')
	output.write(struct.pack('H', packet))
	output.close()
	#TODO add timeout and retry protocol
	if(expectedResponse):
		return 'something'
