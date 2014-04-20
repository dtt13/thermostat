import sys
sys.path.append('/mnt/sda1/arduino')
from message_passing import writeText, streamImage

__WHITE = 0xFFFF
__BLACK = 0x0000
__BACKGROUND = 0x29D5

weatherCond = {'sunny' : 'few.jpg',
			   'cloudy' : 'bkn.jpg',
			   'rainy' : 'shra70.jpg'}

def updateCondition(conditions):
	streamImage(weatherCond[conditions], 270, 110)
	writeText(260, 170, __WHITE, __BACKGROUND, 1, conditions)

def updateTemps(currentTemp):
	tempStr = '%s%cF' % (currentTemp, 0xb0) # TODO get the unit?
	writeText(130, 120, __WHITE, __BACKGROUND, 2, tempStr)

def updateWeather(currentTemp, conditions):
	#TODO pull data from database in future
	updateCondition(conditions)
	updateTemps(currentTemp)