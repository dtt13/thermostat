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
	streamImage(weatherCond[conditions], 250, 90)
	writeText(250, 150, __WHITE, __BACKGROUND, 1, conditions)

def updateTemps(currentTemp):
	tempStr = '%s' % currentTemp
	writeText(180, 120, __WHITE, __BACKGROUND, 4, tempStr)

def updateWeather(currentTemp, conditions):
	#TODO pull data from database in future
	updateCondition(conditions)
	updateTemps(currentTemp)