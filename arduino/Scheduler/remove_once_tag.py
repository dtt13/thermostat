#!/usr/bin/env python
import sys, time
from schedule_processing import removeTag

if len(sys.argv) == 2:
	time.sleep(5)
	removeTag(sys.argv[1])