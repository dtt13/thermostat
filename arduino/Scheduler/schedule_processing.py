import subprocess

"""converts a text file into chron file"""

__CRONFILE = "/mnt/sda1/arduino/Scheduler/cronfile.txt"
__SCHEDULE = "/mnt/sda1/arduino/Scheduler/schedule.txt"
__SET_TEMP = "/mnt/sda1/arduino/Scheduler/set_temp.py"
__REMOVE_ONCE = "/mnt/sda1/arduino/Scheduler/remove_once_tag.py"

__DEFAULT_CRON = "* * * * * /mnt/sda1/arduino/ScreenControl/send_time.py &> /mnt/sda1/arduino/ScreenControl/timedate.log\n* * * * * /mnt/sda1/arduino/ScreenControl/send_ip.py\n* * * * * /mnt/sda1/arduino/TempLog/temp_logger.py\n"


# Takes a timeslot string and returns a cron command
def makeEvent(timeslot):
	try:
		(tag,repeatVal,time,temperature,repeat) = timeslot.split('::')
		(Hr,Min) = time.split(':')
		if repeat == "d":
			return Min + " " + Hr + " " + "*" + " " + "*" + " " + "*" + " " + __SET_TEMP + " " + temperature + "\n"
		elif repeat == "w":
			return Min + " " + Hr + " " + "*" + " " + "*" + " " + repeatVal + " " + __SET_TEMP + " " + temperature + "\n"	
		elif repeat == "m":
			return Min + " " + Hr + " " + repeatVal + " " + "*" + " " + "*" + " " + __SET_TEMP + " " + temperature + "\n"	
		elif repeat == "o":
			Month = repeatVal.split('/')[0]
			Day = repeatVal.split('/')[1]
			return Min + " " + Hr + " " + Day + " " + Month + " " + "*" + " " + __SET_TEMP + " " + temperature + "\n" + Min + " " + Hr + " " + Day + " " + Month + " " + "*" + " " + __REMOVE_ONCE + " " + tag + "\n"
	except ValueError:
		pass
	return ""

# Makes a cronfile from the schedule
def makeCron():
	sch = open(__SCHEDULE, 'r')
	cron = open(__CRONFILE, 'w')
	cron.write(__DEFAULT_CRON)
	timeslots = sch.readline().split(';')
	for slot in timeslots:
		cron.write(makeEvent(slot))
	sch.close()
	cron.close()
	subprocess.call(['crontab', __CRONFILE])

# Removes a tag from the schedule and updates the cronfile
def removeTag(tagToRemove):
	sch = open(__SCHEDULE, 'r')
	timeslots = sch.readline().split(';')
	sch.close()
	out = open(__SCHEDULE, 'w')
	for slot in timeslots:
		try:
			(tag,repeatVal,time,temperature,repeat) = slot.split('::')
			if tag != tagToRemove:
				out.write(slot + ';')
		except ValueError:
			pass
	out.close()
	makeCron()
