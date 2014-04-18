from message_passing import *

setTargetTemp(75)
while getMode() != 'heating':
	switchMode()

