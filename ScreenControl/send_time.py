#!/usr/bin/env python

from time import strftime
import sys
sys.path.append('/mnt/sda1/arduino')
from message_passing import *

__WHITE = 0xFFFF
__BLACK = 0x0000

print 'sending time and date...'

timeDate = strftime('%a %m-%d-%Y %I:%M%p')
print timeDate

if writeText(155, 5, __WHITE, __BLACK, 0, timeDate) == 'error':
	print 'error sending time to screen'
