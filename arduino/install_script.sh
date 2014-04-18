#!/bin/bash
# roost_install.sh

YUN=/mnt/sda1
ARD=$YUN/arduino
PACK=$YUN/packages
PYTHON=$PACK/bin/python
PIP=$PACK/bin/pip

opkg update
mkdir $PACK

# install easy_install and pip which is used for installing other packages
opkg install distribute
opkg install python-openssl
easy_install pip

# install virtualenv
echo "installing virtualenv..."
easy_install virtualenv
echo "creating virtual environment..."
virtualenv $PACK

# install the PIL package
echo "installing PIL..."
opkg install python-imaging-library

# install the web.py package
echo "installing web.py..."
$PIP install web.py

# install suds
echo "installing suds..."
$PIP install suds

# install lxml
echo "installing lxml..."
$PIP install lxml

echo "all done."