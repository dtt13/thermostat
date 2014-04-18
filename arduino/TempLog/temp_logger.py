#!/usr/bin/env python
import sys, os, time
from datetime import datetime, timedelta
sys.path.append('/mnt/sda1/arduino')
from message_passing import *

currentTime = time.strftime('%I:%M%p')
targetTemp = getTargetTemp()
roomTemp = getRoomTemp()
units = getUnit()
if units == 'error':
	units = ''
on = isOn()

pathname = '/mnt/sda1/arduino/TempLog/'
filename = '%stemp%s.log' % (pathname, time.strftime('%m%d'))

if targetTemp != 'error' or roomTemp != 'error' and on != 'error':
	data = '%s\ntarget temperature:\t%d%s\nroom temperature:\t%d%s\nsystem is currently ' % (currentTime, targetTemp, units, roomTemp, units)
	if on:
		data = data + 'on\n\n'
	else:
		data = data + 'off\n\n'
else:
	data = '%s\nERROR\n\n' % (currentTime)

if os.path.isfile(filename):
	f = open(filename, 'a')
else:
	oldFilename = '%stemp%s.log' % (pathname, time.strftime('%m%d', (datetime.now() - timedelta(days=30)).timetuple()))
	if os.path.isfile(oldFilename):
		os.remove(oldFilename)
	f = open(filename, 'w')

f.write(data)
f.close()
