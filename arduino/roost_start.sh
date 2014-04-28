#!/bin/ash
# roost_start.sh

PYTHON=/mnt/sda1/packages/bin/python
ARD=/mnt/sda1/arduino

# kill the bridge daemon
echo
echo "Killing bridge..."
kill-bridge

# setup executables
echo
echo "Setting up executables..."
chmod +x $ARD/ScreenControl/send_ip.py
chmod +x $ARD/ScreenControl/send_time.py
chmod +x $ARD/Scheduler/remove_once_tag.py
chmod +x $ARD/Scheduler/set_temp.py
chmod +x $ARD/TempLog/temp_logger.py
chmod +x $ARD/WeatherApp/weather.py

# start up the website
echo
echo "Starting webpage..."
cd /mnt/sda1/arduino/Webpage
$PYTHON bin/app.py &

# start the weather application
echo
echo "Starting weather app..."
cd /mnt/sda1/arduino/WeatherApp
$PYTHON weather.py &

echo
echo "All done roost start up."