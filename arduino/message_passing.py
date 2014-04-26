

import io, struct, select, time, os, re
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
__GET_TEMP 				= 'G'
__GET_MODE 				= 'H'
__GET_UNIT				= 'J'
__IS_ON					= 'O'
__SET_TARGET_TEMP 		= 'S'
__SWITCH				= 'T'
__WRITE_TEXT			= 'W'
__STREAM_IMAGE			= 'P'
__CLEAR_APP				= 'C'
__APP_TOUCH				= 'A'
__SET_IP				= 'I'

# Buffer size on the Arduino
__BUFF_SIZE				= 1200


# Gets the temperature of the room
def getRoomTemp():
	result = __sendCommandWithRetry(__GET_TEMP, '', True)
	if len(result) == 4:
		(roomTemp, targetTemp) = struct.unpack('<HH', result)
		return roomTemp
	else:
		return 'error'

# Gets the target temperature
def getTargetTemp():
	result = __sendCommandWithRetry(__GET_TEMP, '', True)
	if len(result) == 4:
		(roomTemp, targetTemp) = struct.unpack('<HH', result)
		return targetTemp
	else:
		return 'error'

# Gets the current mode of operation (heating or cooling)
def getMode():
	result = __sendCommandWithRetry(__GET_MODE, '', True)
	if result == 'h': 
		return 'heating'
	elif result == 'c':
		return 'cooling'
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
	return (result[0] == 't')

# Return True if the fan is on, False otherwise
def isFanOn():
	result = __sendCommandWithRetry(__IS_ON, '', True)
	return (result[1] == 't')

# Sets the target temperature
def setTargetTemp(newTargetTemp):
	# tempString = '%d' % newTargetTemp
	temp = struct.pack('h', newTargetTemp)
	return __sendCommandWithRetry(__SET_TARGET_TEMP, temp, False)

# Switches the system between heating and cooling
def switchMode():
	return __sendCommandWithRetry(__SWITCH, 'm', False)

# Switches the temperature units
def switchUnit():
	return __sendCommandWithRetry(__SWITCH, 'u', False)

# Switches the fan override
def switchFan():
	return __sendCommandWithRetry(__SWITCH, 'f', False)

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

# Clears the contents of the application area
def clearApp():
	return __sendCommandWithRetry(__CLEAR_APP, '', False)

def isAppTouched():
	result = __sendCommandWithRetry(__APP_TOUCH, '', True)
	if len(result) == 4:
		(x, y) = struct.unpack('<HH', result)
		return (True, x, y)
	else:
		return (False, 0, 0)

# Sends the Yun's IP address to the screen
def setIP():
	pat = re.compile(r"inet addr:(?P<ip>\d+\.\d+\.\d+\.\d+)")
	ip = os.popen('ifconfig wlan0 | grep -o "inet addr:[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}"').readline()
	match = pat.match(ip)
	if match:
		ip = match.group('ip')
	else:
		ip = 'Not Connected'
	return __sendCommandWithRetry(__SET_IP, ip, False)

# Sends a command to the microcontroller with the message
# expectedResponse should be True if expecting a response
# from the microcontroller, False otherwise
def __sendCommandWithRetry(command, message, expectedResponse, maxTrials = __MAX_TRIALS, timeout = __TIMEOUT):
	packetSize = struct.pack('H', len(message))
	packet = '%s%s%s' % (command, packetSize, message)
	# transmit data and wait for response
	trial = 0
	success = False
	while (not success) and (trial < maxTrials):
		ser_out = io.open(__SERIAL1, 'wb')
		ser_out.write(packet)
		ser_out.close()
		ser_in = io.open(__SERIAL1, 'rb')
		rlist, wlist, xlist = select.select([ser_in], [], [], timeout)
		for reader in rlist:
			response = reader.readline().rstrip('\r\n')
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