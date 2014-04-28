#!/bin/bash
# roost_install.sh

YUN=/mnt/sda1
ARD=$YUN/arduino
PACK=$YUN/packages
PYTHON=$PACK/bin/python
PIP=$PACK/bin/pip
CRON=/etc/init.d/cron

opkg update
mkdir $PACK


# # install easy_install and pip which is used for installing other packages
echo
echo "Installing dependencies..."
opkg install distribute
opkg install python-openssl

# install the PIL package
echo
echo "Installing PIL..."
opkg install python-imaging-library


# install virtualenv
echo
echo "Installing virtualenv..."
easy_install virtualenv
echo "Creating virtual environment..."
virtualenv --system-site-packages $PACK

# install the web.py package
echo
echo "Installing web.py..."
# $PIP install web.py
cd $PACK
wget http://webpy.org/static/web.py-0.37.tar.gz
tar xvzf web.py-0.37.tar.gz
cd $PACK/web.py-0.37
$PYTHON setup.py install
cd $PACK
rm -rf $PACK/web.py-0.37.tar.gz $PACK/web.py-0.37

#install PHP
echo
echo "Installing PHP..."
opkg install php5-cli
opkg install php5-mod-soap

# # install suds
# echo
# echo "Installing suds..."
# $PIP install suds

# # install ElementTree
# echo
# echo "Installing ElementTree..."
# # $PIP install lxml
# wget http://effbot.org/media/downloads/elementtree-1.2.6-20050316.tar.gz
# tar xvzf elementtree-1.2.6-20050316.tar.gz
# cd $PACK/elementtree-1.2.6-20050316
# $PYTHON setup.py install
# cd $PACK
# rm -rf $PACK/elementtree-1.2.6-20050316.tar.gz $PACK/elementtree-1.2.6-20050316

# start up cron
echo
echo "Setting up cron..."
$CRON start
$CRON enable

# change necessary files to executables
echo
echo "Setting up executables..."
chmod +x $ARD/ScreenControl/send_ip.py
chmod +x $ARD/ScreenControl/send_time.py
chmod +x $ARD/Scheduler/remove_once_tag.py
chmod +x $ARD/Scheduler/set_temp.py
chmod +x $ARD/TempLog/temp_logger.py
chmod +x $ARD/WeatherApp/weather.py
chmod +x $ARD/roost_start.sh

echo
echo "All Done."