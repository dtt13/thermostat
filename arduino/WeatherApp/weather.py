import sys
import ndfd_control
sys.path.append('/mnt/sda1/arduino')
from message_passing import writeText, streamImage

__WHITE = 0xFFFF
__BLACK = 0x0000
__BACKGROUND = 0x29D5

data = {}

def updateCondition(conditions):
	streamImage(weatherCond[conditions], 270, 110)
	while len(conditions) < 6:
		conditions += ' '
	writeText(260, 170, __WHITE, __BACKGROUND, 1, conditions)

def updateTemps(currentTemp):
	tempStr = '%s%cF' % (currentTemp, 0xb0) # TODO get the unit?
	writeText(130, 120, __WHITE, __BACKGROUND, 2, tempStr)

def updateWeather():
	global data = ndfd_control.update_weather_data()
	for i in data:
		print i, data[i]
	#TODO pull data from database in future
	# updateCondition(conditions)
	# updateTemps(currentTemp)