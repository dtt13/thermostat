#!/usr/bin/env python
import sys
from schedule_processing import removeTag

if len(sys.argv) == 2:
	removeTag(sys.argv[1])