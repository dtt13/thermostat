from message_passing import *
import time

i = 2**10
maxsize = 2**12
while (i <= maxsize):
	print 'testing data size: ', i
	# start_time = time.clock()
	if testStream(i) == 'error':
		print 'error'
	end_time = time.clock()
	# print 'time elapsed: %d' % (end_time - start_time)
	time.sleep(4)
	i *= 2 