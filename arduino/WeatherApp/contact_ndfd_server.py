###############################################################
# For information regarding the function of basic SUDS methods#
# (client, service, factory, etc), please see the online wiki #
# at https://fedorahosted.org/suds/wiki/Documentation         #
###############################################################

###############################################################
# Necessary imports. 'logging' is used for debugging only,    #
# 'datetime' is used to generate timestamps for requests, and #
# 'suds.client' is the SUDS client application.               #
###############################################################
import logging #uncomment for debugging
import re
from datetime import datetime, timedelta
from suds.client import Client

# Set up logger (comment all of this unless debugging)
logging.basicConfig(level=logging.INFO)
logging.getLogger('Client').setLevel(logging.DEBUG)
#logging.getLogger('suds.client').setLevel(logging.DEBUG)
#logging.getLogger('suds.transport').setLevel(logging.DEBUG)
#logging.getLogger('suds.xsd.schema').setLevel(logging.DEBUG)
#logging.getLogger('suds.wsdl').setLevel(logging.DEBUG)

###############################################################
# Create a client and initialize the request parameters       #
###############################################################
# Set the url (this is static, as it's the address of the
# NDFD database)
url = 'http://graphical.weather.gov/xml/DWMLgen/wsdl/ndfdXML.wsdl'
# Use the url to create a client object
# 'faults=False' makes responses arrive in (status, response)
# tuples, with the status code (eg, '200') as the first item.
bubble = Client(url, faults=False)
# Create an object that matches the possible parameters for the
# online service. These settings control what info we want to
# get back from the NDFD server
bubble.soap_parameters = bubble.factory.create('ns0:weatherParametersType')
# Create a list of the settings we want. The list of possible
# options can be found by printing the client, or at
# graphical.weather.gov/xml/docs/elementInputNames.php
s_param = ['maxt', #maximum temperature 
           'mint', #minimum temperature
           'temp', #3 hourly temperature (1st value is current temp)
           'pop12',#12 hour probability of precipitation
           'qpf',  #Liquid precipitation amount
           'appt', #Apparent temperature
           'rh',   #relative humidity
           'wx',   #weather
           'icons',#weather icons
           'wwa']  #weather warnings and advisories
#s_param = ['maxt', 'mint', 'sky', 'wx', 'icons', 'wwa']
# Update the settings object. (ie, set the fields we have
# specified in our settings array to True in the actual
# parameter object.
for prm in s_param:
    if hasattr(bubble.soap_parameters, prm):
        bubble.soap_parameters[prm] = True
    else:
        raise AttributeError("Atr. '%s' doesn't exist." % prm)
# Now, set everything we don't want to false (the ndfd
# server sometimes has errors if you leave something blank)
for prm, i in bubble.soap_parameters:
    if bubble.soap_parameters[prm] != True:
        bubble.soap_parameters[prm] = False
# We want the weather information for today, so we're
# finding the current date for the start time, then
# setting the end time three hours from now. (this is
# arbitrary, and was simply decided by us)
curDate = datetime.now().isoformat()
daysEnd = (datetime.now() + timedelta(hours=3)).isoformat()
###############################################################
# Send the request to the server and store the                #
# XML response. The response is in the form of a tuple,       #
# due to faults being set to False when the client is         #
# created. The first value in the tuple is simply the         #
# request status returned by the server.                      #
###############################################################
#Get the zip code from the user file
zc_file = open("zip_code.txt", "r")
zcode = zc_file.read()
zc_file.close()
#Get the lat/lon from zipcode
(stat, res_zc) = bubble.service.LatLonListZipCode(zcode)
zc_match = re.search('<latLonList>(.*)</latLonList>', res_zc)
if zc_match:
    (lat, lon) = zc_match.group(1).split(',')
    #print ("lat: " + lat + " lon: " + lon)
else:
    exit() # There was some problem loading the zc, so don't make a request
#Send the actual request
(status, result) = bubble.service.NDFDgen(
        latitude = lat,
        longitude = lon,
        product = 'time-series',
        startTime = curDate,
        endTime = daysEnd,
        Unit = 'e',
        weatherParameters = bubble.soap_parameters)
# Open an xml file and save the response xml, from
# the server, in it.
xml_file = open("ndfd_info.xml", "w")
xml_file.write(str(result))
xml_file.close()