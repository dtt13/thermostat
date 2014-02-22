from message_passing import *

expectedRoomTemp = getRoomTemp()
expectedTargetTemp = getTargetTemp()
expectedIncTemp = expectedTargetTemp + 1
expectedDecTemp = expectedTargetTemp
expectedMode = 'heating'
expectedSwitchMode = 'cooling'
expectedUnit = 'f'
# expectedSwitchUnit = 'c'
expectedIsOn = isOn()
expectedSetTarget = expectedTargetTemp + 5

error = False

for i in range(0,5000):
	print 'trial', (i+1)
	# Test getting and setting target temp
	setTargetTemp(expectedSetTarget)
	if(getTargetTemp() != expectedSetTarget):
		print 'Error! Did not set target temp up'
		error = True
	setTargetTemp(expectedTargetTemp)
	if(getTargetTemp() != expectedTargetTemp):
		print 'Error! Did not set target temp down'
		error = True
	# Test incrementing and decrementing the target temp
	incrementTargetTemp()
	if(getTargetTemp() != expectedIncTemp):
		print 'Error! Did not increment target temp'
		error = True
	decrementTargetTemp()
	if(getTargetTemp() != expectedDecTemp):
		print 'Error! Did not decrement target temp'
		error = True
	# Test getting the room temp
	if(getRoomTemp() != expectedRoomTemp):
		print 'Error! Did not get the correct room temp'
		error = True
	# Test getting and switching modes
	switchMode()
	if(getMode() != expectedSwitchMode):
		print 'Error! Did not switch mode to cooling'
		error = True
	switchMode()
	if(getMode() != expectedMode):
		print 'Error! Did not switch mode to heating'
		error = True
	# Test getting unit
	if(getUnit() != expectedUnit):
		print 'Error! Did not get the correct unit'
		error = True
	# Test if the system is on
	if(isOn() != expectedIsOn):
		print 'Error! System should not have turned on or off'
		error = True

	if not error:
		print 'success!'
	else:
		break
