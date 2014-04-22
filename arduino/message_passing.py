

import io, struct, select, time
from PIL import Image

""" This library is used to send simple messages to and
	from the microcontroller. Features include:
	-thermostat control
	-touchscreen control
	-image streaming
"""

# Serial1 connection
__SERIAL1 = '/dev/ttyATH0'

# Timeout and maximum number of trials
__TIMEOUT = 1 # seconds
__MAX_TRIALS = 3

# Message commands constants
__GET_ROOM_TEMP 		= 'R'
__GET_TARGET_TEMP 		= 'G'
__GET_MODE 				= 'H'
__GET_UNIT				= 'J'
__IS_ON					= 'O'
__SET_TARGET_TEMP 		= 'S'
#__INCREMENT_TARGET_TEMP = 'I'
#__DECREMENT_TARGET_TEMP = 'D'
__SWITCH_MODE			= 'M'
__SWITCH_UNIT			= 'U'
__WRITE_TEXT			= 'W'
__STREAM_IMAGE			= 'P'

# Buffer size on the Arduino
__BUFF_SIZE				= 1536


# Gets the temperature of the room
def getRoomTemp():
	result = __sendCommandWithRetry(__GET_ROOM_TEMP, '', True)
	if(result.isdigit()):
		return int(result)
	else:
		return 'error'

# Gets the target temperature
def getTargetTemp():
	result = __sendCommandWithRetry(__GET_TARGET_TEMP, '', True)
	if(result.isdigit()):
		return int(result)
	else:
		return 'error'

# Gets the current mode of operation (heating or cooling)
def getMode():
	result = __sendCommandWithRetry(__GET_MODE, '', True)
	if(result == 'heating' or result == 'cooling'):
		return result
	else:
		return 'error'

# Gets the temperature units (f or c)
def getUnit():
	result = __sendCommandWithRetry(__GET_UNIT, '', True)
	if(result == 'f' or result == 'c'):
		return result
	else:
		return 'error'

# Return True if the system is on, False otherwise
def isOn():
	result = __sendCommandWithRetry(__IS_ON, '', True)
	return (result == 'true')

# Sets the target temperature
def setTargetTemp(newTargetTemp):
	# tempString = '%d' % newTargetTemp
	temp = struct.pack('h', newTargetTemp)
	return __sendCommandWithRetry(__SET_TARGET_TEMP, temp, False)

# # Increase the target temperature by one degree
# def incrementTargetTemp():
# 	return __sendCommandWithRetry(__INCREMENT_TARGET_TEMP, '', False)

# # Decrease the target temperature by one degree
# def decrementTargetTemp():
# 	return __sendCommandWithRetry(__DECREMENT_TARGET_TEMP, '', False)

# Switches the system between heating and cooling
def switchMode():
	return __sendCommandWithRetry(__SWITCH_MODE, '', False)

# Switches the temperature units
def switchUnit():
	return __sendCommandWithRetry(__SWITCH_UNIT, '', False)

# Sends text to be displayed on the screen
def writeText(x, y, fColor, bColor, fontSize, text):
	packet = '%s%s' % (struct.pack('HHHHB', x, y, fColor, bColor, fontSize), text)
	return __sendCommandWithRetry(__WRITE_TEXT, packet, False)

# Sends an image touchscreen at the specified location
def streamImage(file, xpos, ypos):
	image = Image.open(file)
	width, height = image.size
	# prep image for sending
	r, g, b = image.convert('RGB').split()
	R = list(r.getdata())
	G = list(g.getdata())
	B = list(b.getdata())
	imagePixels = R
	for i in range(len(R)):
		imagePixels[i] = ((R[i] & 0xF8) << 8) | ((G[i] & 0xFC) << 3) | (B[i] >> 3)
		imagePixels[i] = ((0x00FF & imagePixels[i]) << 8) | ((0xFF00 & imagePixels[i]) >> 8)
	# calculate how much of the image to send per transmission
	headerSize = 11
	rowsPerTx = (__BUFF_SIZE - headerSize) / (width * 2)
	# print 'rows per transmission:', rowsPerTx
	rowCount = 0
	response = ''
	while(response != 'error' and rowCount < height):
		startPixel = rowCount * width
		endPixel = min(height, (rowCount + rowsPerTx)) * width
		message = struct.pack('HHHH' + (endPixel-startPixel) * 'H', xpos, ypos + rowCount, width, (endPixel / width) - rowCount, *imagePixels[startPixel:endPixel])
		response = __sendCommandWithRetry(__STREAM_IMAGE, message, False, 1, 1.5)
		rowCount += rowsPerTx
	if(response == 'error'):
		return 'error'

# Sends a command to the microcontroller with the message
# expectedResponse should be True if expecting a response
# from the microcontroller, False otherwise
def __sendCommandWithRetry(command, message, expectedResponse, maxTrials = __MAX_TRIALS, timeout = __TIMEOUT):
	packetSize = struct.pack('H', len(message))
	packet = '%s%s%s' % (command, packetSize, message)
	# empty the serial port before sending messages
	# __emptySerial()
	# transmit data and wait for response
	trial = 0
	success = False
	while (not success) and (trial < maxTrials):
		# print 'trial #:', trial
		ser_out = io.open(__SERIAL1, 'wb')
		ser_out.write(packet)
		ser_out.close()
		ser_in = io.open(__SERIAL1, 'rb')
		rlist, wlist, xlist = select.select([ser_in], [], [], timeout)
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
# Does not wait for any response before terminating only sends the
# packet once
def __sendCommandWithoutRetry(command, message):
	packetSize = struct.pack('H', len(message))
	packet = '%s%s%s' % (command, packetSize, message)
	ser_out = io.open(__SERIAL1, 'wb')
	ser_out.write(packet)
	ser_out.close()

def __emptySerial():
	ser_in = io.open(__SERIAL1, 'rb')
	ser_in.readall()
	print 'done emptying serial'
	ser_in.close()