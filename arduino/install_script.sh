#!/bin/bash
# roost_install.sh

YUN=/mnt/sda1
ARD=$YUN/arduino
PACK=$YUN/packages
PYTHON=$PACK/bin/python
PIP=$PACK/bin/pip

opkg update
mkdir $PACK

# install the PIL package
echo
echo "Installing PIL..."
opkg install python-imaging-library

# install easy_install and pip which is used for installing other packages
echo
echo "Installing pip..."
opkg install distribute
opkg install python-openssl
easy_install pip

# install virtualenv
echo
echo "Installing virtualenv..."
easy_install virtualenv
echo "Creating virtual environment..."
virtualenv $PACK

# install the web.py package
echo
echo "Installing web.py..."
$PIP install web.py

# install suds
echo
echo "Installing suds..."
$PIP install suds

# install lxml
# echo
# echo "installing lxml..."
# $PIP install lxml

echo
echo "All Done."