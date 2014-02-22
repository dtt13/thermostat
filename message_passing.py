

import io, struct, select
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
__MAX_TRIALS = 3

# Message commands constants
__GET_ROOM_TEMP 		= 'R'
__GET_TARGET_TEMP 		= 'G'
__GET_MODE 				= 'H'
__GET_UNIT				= 'J'
__IS_ON					= 'O'
__SET_TARGET_TEMP 		= 'S'
__INCREMENT_TARGET_TEMP = 'I'
__DECREMENT_TARGET_TEMP = 'D'
__SWITCH_MODE			= 'M'
__SWITCH_UNIT			= 'U'

# Gets the temperature of the room
def getRoomTemp():
	result = __sendCommand(__GET_ROOM_TEMP, '', True)
	if(result.isdigit()):
		return int(result)
	else:
		return 'error'

# Gets the target temperature
def getTargetTemp():
	result = __sendCommand(__GET_TARGET_TEMP, '', True)
	if(result.isdigit()):
		return int(result)
	else:
		return 'error'

# Gets the current mode of operation (heating or cooling)
def getMode():
	result = __sendCommand(__GET_MODE, '', True)
	if(result == 'heating' or result == 'cooling'):
		return result
	else:
		return 'error'

# Gets the temperature units (f or c)
def getUnit():
	result = __sendCommand(__GET_UNIT, '', True)
	if(result == 'f' or result == 'c'):
		return result
	else:
		return 'error'

# Return True if the system is on, False otherwise
def isOn():
	result = __sendCommand(__IS_ON, '', True)
	return (result == 'true')

# Sets the target temperature
def setTargetTemp(newTargetTemp):
	tempString = '%d' % newTargetTemp
	return __sendCommand(__SET_TARGET_TEMP, tempString, False)

# Increase the target temperature by one degree
def incrementTargetTemp():
	return __sendCommand(__INCREMENT_TARGET_TEMP, '', False)

# Decrease the target temperature by one degree
def decrementTargetTemp():
	return __sendCommand(__DECREMENT_TARGET_TEMP, '', False)

# Switches the system between heating and cooling
def switchMode():
	return __sendCommand(__SWITCH_MODE, '', False)

# Switches the temperature units
def switchUnit():
	return __sendCommand(__SWITCH_UNIT, '', False)

# Sends a command to the microcontroller with the message
# expectedResponse should be True if expecting a response
# from the microcontroller, False otherwise
def __sendCommand(command, message, expectedResponse):
	# return __sendCommandSerial(command, message, expectedResponse)
	packet = '%s%s\n' % (command, message)
	# transmit data and wait for response
	trial = 0
	success = False
	# TODO add in handshake so no duplicate packets?
	while (not success) and (trial < __MAX_TRIALS):
		ser_out = io.open(__SERIAL1, 'wb')
		# ser_out.write(struct.pack('%ds' % len(packet), packet))
		ser_out.write(packet)
		ser_out.close()
		ser_in = io.open(__SERIAL1, 'rb')
		rlist, wlist, xlist = select.select([ser_in], [], [], __TIMEOUT)
		for reader in rlist:
			response = reader.readline().rstrip('\r\n')
			# print response
			success = (len(response) > 0) and (response[0] == command)
		trial += 1
		ser_in.close()

	if success:
		if expectedResponse:
			return response[1:]
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

	ser.close()
	
	if success:
		if expectedResponse:
			return response[1:]
	else:
		return 'error'


