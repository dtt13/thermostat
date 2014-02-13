

import io, struct
import serial

""" This library is used to send simple messages to and
	from the microcontroller. Features include:
	-thermostat control
	-touchscreen control
	-image streaming
"""

# Serial1 connection
__SERIAL1 = '/dev/ttyATH0'

# Timeout and maximum number of trials
__BAUD = 9600 # will increase this in the future
__TIMEOUT = 1 # seconds
__MAX_TRIALS = 1

# Message commands constants
__SET_TARGET_TEMP = 'S'
__GET_TARGET_TEMP = 'G'
__GET_ROOM_TEMP = 'R'
__INCREMENT_TARGET_TEMP = 'I'
__DECREMENT_TARGET_TEMP = 'D'

# Sets the target temperature
def setTargetTemp(newTargetTemp):
	__sendCommand(__SET_TARGET_TEMP, newTargetTemp, False)

# Gets the target temperature
def getTargetTemp():
	result = __sendCommand(__GET_TARGET_TEMP, '', True)
	return result

# Gets the temperature of the room
def getRoomTemp():
	__sendCommand(__GET_ROOM_TEMP, '', True)

# Increase the target temperature by one degree
def incrementTargetTemp():
	__sendCommand(__INCREMENT_TARGET_TEMP, '', False)

# Decrease the target temperature by one degree
def decrementTargetTemp():
	__sendCommand(__DECREMENT_TARGET_TEMP, '', False)

# Sends a command to the microcontroller with the message
# expectedResponse should be True if expecting a response
# from the microcontroller, False otherwise
def __sendCommand(command, message, expectedResponse):
	# return __sendCommandSerial(command, message, expectedResponse)
	packet = '%s%s\n' % (command, message)
	# ser_in = io.open(__SERIAL1, 'rb')
	# ser_out = io.open(__SERIAL1, 'wb')
	# transmit data and wait for response
	trial = 0
	success = False
	# TODO add timeout with select here!!!
	# TODO add in handshake so no duplicate packets?
	while (not success) and (trial < __MAX_TRIALS):
		ser_out = io.open(__SERIAL1, 'wb')
		ser_out.write(struct.pack('s', packet))
		ser_out.close()
		ser_in = io.open(__SERIAL1, 'rb')
		response = ser_in.readline()
		ser_in.close()
		success = (len(response) > 0) and (response[0] == command)
		trial += 1

	if expectedResponse:
		if success:
			return response
		else:
			return 'error'

# Sends a command to the microcontroller with the message
# DOES NOT WORK WITH SERIAL LIB FOR STRANGE UNKNOWN REASON!
def __sendCommandSerial(command, message, expectedResponse):
	packet = '%s%s\n' % (command, message)
	ser = serial.Serial(__SERIAL1, __BAUD, timeout=__TIMEOUT)
	# transmit data and wait for response
	trial = 0
	success = False
	while (not success) and (trial < __MAX_TRIALS):
		ser.write(packet)
		print 'trail: %d' % trial
		response = ser.readline()
		print response
		success = (len(response) > 0) and (response[0] == command)
		trial += 1

	# increment/decrement!!!
	if expectedResponse:
		return response


