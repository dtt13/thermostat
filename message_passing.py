

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
#__INCREMENT_TARGET_TEMP = 'I'
#__DECREMENT_TARGET_TEMP = 'D'
__SWITCH_MODE			= 'M'
__SWITCH_UNIT			= 'U'
__WRITE_TEXT			= 'W'
__STREAM_IMAGE			= 'P'

# Image streaming sub-commands
__STREAM_INIT			= 'i'
__STREAM_SEND			= 's'
__STREAM_FIN			= 'f'

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
	# send an image command
	message = struct.pack('cHHHH', __STREAM_INIT, xpos, ypos, width, height)
	response = __sendCommandWithRetry(__STREAM_IMAGE, message, True)
	print response
	if response == 'error' or len(response) != 4:
		return 'error'
	else:
		bufferSize, firstPixel = struct.unpack('HH', response)
		firstPixel *= 8
	# send image file
	if __stream(imagePixels, firstPixel, bufferSize) == 'error':
		return 'error'
	# send termination command
	__sendCommandWithoutRetry(__STREAM_IMAGE, __STREAM_FIN)

# Sends a command to the microcontroller with the message
# expectedResponse should be True if expecting a response
# from the microcontroller, False otherwise
def __sendCommandWithRetry(command, message, expectedResponse):
	packet = '%s%s\n' % (command, message)
	# transmit data and wait for response
	trial = 0
	success = False
	# TODO add in handshake so no duplicate packets?
	while (not success) and (trial < __MAX_TRIALS):
		ser_out = io.open(__SERIAL1, 'wb')
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
# Does not wait for any response before terminating only sends the
# packet once
def __sendCommandWithoutRetry(command, message):
	packet = '%s%s\n' % (command, message)
	ser_out = io.open(__SERIAL1, 'wb')
	ser_out.write(packet)
	ser_out.close()

# Sends an image to the microcontroller using an ACK-based protocol
# May error out if the microcontroller does not respond
def __stream(data, firstPixel, bufferSize):
	headerSize = 6 # bytes
	pixelsPerTx = int((bufferSize - headerSize) / (2 * 8)) * 8 # get a multiple of 8 pixels
	pixelExpected = firstPixel # pixels
	totalPixels = len(data) # pixels
	trial = 0
	print pixelExpected, totalPixels
	while (pixelExpected < totalPixels) and (trial < __MAX_TRIALS):
		print 'sending pixel %d' % pixelExpected

		success = False
		pixelsToTx = min(int((totalPixels - pixelExpected) / 8) * 8, pixelsPerTx)
		# write to fill up the buffer or end of file
		ser_out = io.open(__SERIAL1, 'wb')
		ser_out.write(struct.pack('ccHH', __STREAM_IMAGE, __STREAM_SEND, pixelExpected / 8, pixelsToTx / 8))
		for i in range(pixelExpected, pixelsToTx + pixelExpected):
			# print hex(data[i])
			ser_out.write(struct.pack('H', data[i])) #TODO pad the end with zeros if needed
		ser_out.close()
		time.sleep(0.1)
		# read back response
		ser_in = io.open(__SERIAL1, 'rb')
		rlist, wlist, xlist = select.select([ser_in], [], [], __TIMEOUT)
		for reader in rlist:
			response = reader.readline().rstrip('\r\n')
			if (len(response) == 3) and (response[0] == __STREAM_IMAGE):
				pixelExpected, = struct.unpack('H', response[1:3])
				pixelExpected *= 8
				success = True
		if success:
			trial = 0
		else:
			trial += 1
		ser_in.close()
	if(trial == __MAX_TRIALS):
		return 'error'

# Sends an image to the microcontroller with little to no checking
def __stream2(data, bufferSize):
	pixelsPerTx = int((bufferSize - 2) / 2)	
	nextPixel = 0
	while(nextPixel < len(data)):
		ser_out = io.open(__SERIAL1, 'wb')
		ser_out.write(struct.pack('cc', __STREAM_IMAGE, __STREAM_SEND))
		for i in range(nextPixel, min(nextPixel + pixelsPerTx, len(data))):
			ser_out.write(struct.pack('H', data[i]))
		ser_out.close()
		ser_in = io.open(__SERIAL1, 'rb')
		rlist, wlist, xlist = select.select([ser_in], [], [], 3) #longer timeout
		for reader in rlist:
			response = reader.readline().rstrip('\r\n')
			if (len(response) > 0) and (response[0] == __STREAM_IMAGE):
				nextPixel += pixelsPerTx
		ser_in.close()
		
#Used for testing only
def testStream(data_size):
	data = []
	for i in range(data_size):
		data.append(i+1)
	# send an image command
	print 'sending init...'
	message = struct.pack('cHHHH', __STREAM_INIT, 32, 32, 8, len(data) / 8)
	response = __sendCommandWithRetry(__STREAM_IMAGE, message, True)
	#print len(response)
	if response == 'error' or len(response) != 4:
		return 'error'
	else:
		bufferSize, firstPixel = struct.unpack('HH', response)
		# print firstPixel, bufferSize
		firstPixel *= 8

	# send image file
	print 'sending image...'
	# if __stream(data, firstPixel, bufferSize) == 'error':
	# 	return 'error'
	__stream2(data, bufferSize)

	# send termination command
	#print 'sending temination...'
	#__sendCommandWithoutRetry(__STREAM_IMAGE, __STREAM_FIN)

