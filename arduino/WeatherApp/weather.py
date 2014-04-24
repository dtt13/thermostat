import sys
import ndfd_control
sys.path.append('/mnt/sda1/arduino')
from message_passing import writeText, streamImage, clearApp

__DEGREE_SYM = 0xb0

__WHITE = 0xFFFF
__BLACK = 0x0000
__BACKGROUND = 0x29D5
__ADVIS_COLOR = 0x0000 #TODO
__WARN_COLOR = 0x0000 #TODO

__CURRENT_TEMP = 'Daily Temperature'
__MAX_TEMP = 'Daily Maximum Temperature'
__MIN_TEMP = 'Daily Minimum Temperature'
__PRECIP = '12 Hourly Probability of Precipitation'
__ICON = 'Conditions Icon'
__CONDITIONS = 'Weather Type, Coverage, Intensity'
__ADVISORY = 'Weather Advisory'
__WARNING = 'Weather Warning'

data = {}

# Draws the conditions icon and writes the conditions text
def updateConditions():
	if __ICON in data:
		streamImage('icon.jpg', 270, 110)
	if __CONDITIONS:
		writeText(270, 200, __WHITE, __BACKGROUND, 0, data[__CONDITIONS]) #TODO
	# while len(conditions) < 6:
		# conditions += ' '
	# writeText(260, 170, __WHITE, __BACKGROUND, 1, conditions)

# Draws an advisory bar along the top of the application area
def updateAdvisory():
	if __ADVISORY in data:
		streamImage('advisory.jpg', 120, 40)
		writeText(125, 45, __WHITE, __ADVIS_COLOR, 0, data[__ADVISORY])

# Draws a warning bar along the top of the application area
def updateWarning():
	if __WARNING in data:
		streamImage('warning.jpg', 120, 40)
		writeText(125, 45, __WHITE, __WARN_COLOR, 0, data[__WARNING])

# Writes the current, max, min temperatures and chance of precipitation
def updateTemps():
	if __CURRENT_TEMP in data:
		tempStr = '%s%cF' % (data[__CURRENT_TEMP], __DEGREE_SYM)
		writeText(130, 80, __WHITE, __BACKGROUND, 2, tempStr) #TODO on all locations
	if __MAX_TEMP in data:
		tempStr = 'Max: %s%cF' % (data[__MAX_TEMP], __DEGREE_SYM)
		writeText(130, 110, __WHITE, __BACKGROUND, 1, tempStr)
	if __MIN_TEMP in data:
		tempStr = 'Min: %s%cF' % (data[__MIN_TEMP], __DEGREE_SYM)
		writeText(130, 130, __WHITE, __BACKGROUND, 1, tempStr)
	if __PRECIP in data:
		tempStr = 'Precip: %s%' % data[__PRECIP]
		writeText(130, 150, __WHITE, __BACKGROUND, 1, tempStr)

# Draws the weather app's main screen
def updateWeather():
	global data
	data = ndfd_control.update_weather_data()
	if not data:
		clearApp()
		updateAdvisory()
		updateWarning()
		updateConditions()
		updateTemps()