#!/usr/bin/env python

import sys, os.path
import ndfd_control
import map_utils
sys.path.append('/mnt/sda1/arduino')
from message_passing import writeText, streamImage, clearApp, isAppTouched
from time import sleep

# special characters
__DEGREE_SYM = 0xb0

# Screen colors
__WHITE = 0xFFFF
__BLACK = 0x0000
__BACKGROUND = 0x29D5
# __ADVIS_COLOR = 0xF681
__WARN_COLOR = 0xE883

# Weather tags
__CURRENT_TEMP 		= 'Daily Temperature'
__MAX_TEMP 			= 'Daily Maximum Temperature'
__MIN_TEMP 			= 'Daily Minimum Temperature'
__PRECIP 			= '12 Hourly Probability of Precipitation'
__ICON 				= 'Conditions Icon'
__CONDITIONS 		= 'Weather Type, Coverage, Intensity'
__WARNING 			= 'Watches, Warnings, and Advisories'

# States of the weather app
__WEATHER_TODAY		= 1
__WEATHER_MAP		= 2

# Files
__ICON_IMAGE 		= '/mnt/sda1/arduino/WeatherApp/icon.jpg'
__MAP_IMAGE			= '/mnt/sda1/arduino/WeatherApp/map.jpg'

data = {}

# Draws the conditions icon and writes the conditions text
def updateConditions():
	if __ICON in data and os.path.isfile(__ICON_IMAGE):
		streamImage(__ICON_IMAGE, 270, 70)
	if __CONDITIONS in data:
		writeText(140, 140, __WHITE, __BACKGROUND, 0, data[__CONDITIONS]) #TODO

# Draws a warning bar along the top of the application area
def updateWarning():
	if __WARNING in data:
		streamImage('warning.jpg', 120, 40)
		writeText(125, 45, __BLACK, __WARN_COLOR, 0, data[__WARNING])

# Writes the current, max, min temperatures and chance of precipitation
def updateTemps():
	if __CURRENT_TEMP in data:
		tempStr = '%s%cF' % (data[__CURRENT_TEMP], __DEGREE_SYM)
		writeText(140, 80, __WHITE, __BACKGROUND, 2, tempStr) #TODO on all locations
	if __MAX_TEMP in data:
		tempStr = 'Max: %s%cF' % (data[__MAX_TEMP], __DEGREE_SYM)
		writeText(140, 160, __WHITE, __BACKGROUND, 1, tempStr)
	if __MIN_TEMP in data:
		tempStr = 'Min: %s%cF' % (data[__MIN_TEMP], __DEGREE_SYM)
		writeText(140, 190, __WHITE, __BACKGROUND, 1, tempStr)
	if __PRECIP in data:
		tempStr = 'Precip: %s%%' % data[__PRECIP]
		writeText(140, 220, __WHITE, __BACKGROUND, 1, tempStr)

# Draws the weather app's main screen
def updateWeather():
	global data
	data = ndfd_control.update_weather_data()
	if not data:
		clearApp()
		updateWarning()
		updateConditions()
		updateTemps()

# Draws the weather map on the screen
def updateMap():
	if os.path.isfile(__MAP_IMAGE):
		clearApp()
		streamImage(__MAP_IMAGE, 140, 60)

def testTemps():
        global data
        data = {__CURRENT_TEMP : 55, __MAX_TEMP : 66, __MIN_TEMP : 44, __PRECIP : 70}
        updateTemps()

def testConditions():
        global data
        data = {__ICON : 'blah.jpg', __CONDITIONS : 'Chance of Rain'}
        updateConditions()

def testAdvisory():
        global data
        data = {__ADVISORY : 'Wind Advisory till 10PM'}
        updateAdvisory()

def testWarning():
        global data
        data = {__WARNING : 'Thunderstorm Warning'}
        updateWarning()

def testAll():
        clearApp()
        testWarning()
        testConditions()
        testTemps()

#initialize
state = __WEATHER_TODAY
updateWeather()

#process touching
while True:
	(touched, x, y) = isAppTouched()
	if state == __WEATHER_TODAY:
		map_utils.formatMap()
		if touched:
			updateMap()
			state = __WEATHER_MAP
	elif state == __WEATHER_MAP:
		changed = map_utils.formatMap()
		if touched:
			updateWeather()
			state = __WEATHER_TODAY
		elif changed:
			updateMap()
	sleep(1)