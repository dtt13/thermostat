#!/bin/ash
# roost_start.sh

PYTHON=/mnt/sda1/packages/bin/python

# echo "Starting cron"
# /etc/init.d/cron restart
# /etc/init.d/cron start

echo
echo "Killing bridge"
kill-bridge

echo
echo "Starting scheduler"
# TODO

echo
echo "Starting webpage"
cd /mnt/sda1/arduino/Webpage
$PYTHON bin/app.py &

echo
echo "Starting weather app"
cd /mnt/sda1/arduino/WeatherApp
$PYTHON weather.py &

echo "All done roost start up."