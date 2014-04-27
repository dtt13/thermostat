#!/usr/bin/env python

import sys
sys.path.append('/mnt/sda1/arduino')
from message_passing import setTargetTemp

if len(sys.argv) == 2:
	temp = int(sys.argv[1])
	setTargetTemp(temp)