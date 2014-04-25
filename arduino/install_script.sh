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
# easy_install pip

# install the PIL package
echo
echo "Installing PIL..."
opkg install python-imaging-library
# cd $PACK
# wget http://effbot.org/downloads/Imaging-1.1.7.tar.gz
# tar xvzf Imaging-1.1.7.tar.gz
# cd $PACK/Imaging-1.1.7
# $PYTHON setup.py install
# cd $PACK
# rm -rf $PACK/Imaging-1.1.7.tar.gz $PACK/Imaging-1.1.7

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

# install suds
echo
echo "Installing suds..."
$PIP install suds
# cd $PACK
# wget https://pypi.python.org/packages/source/s/suds/suds-0.3.4.tar.gz
# tar xvzf suds-0.3.4.tar.gz
# cd $PACK/suds-0.3.4
# $PYTHON setup.py install
# cd $PACK
# rm -rf $PACK/suds-0.3.4.tar.gz $PACK/suds-0.3.4

# install ElementTree
echo
echo "Installing ElementTree..."
# $PIP install lxml
wget http://effbot.org/media/downloads/elementtree-1.2.6-20050316.tar.gz
tar xvzf elementtree-1.2.6-20050316.tar.gz
cd $PACK/elementtree-1.2.6-20050316
$PYTHON setup.py install
cd $PACK
rm -rf $PACK/elementtree-1.2.6-20050316.tar.gz $PACK/elementtree-1.2.6-20050316

echo
echo "Setting up cron..."
$CRON start
$CRON enable

echo
echo "All Done."