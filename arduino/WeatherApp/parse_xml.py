###############################################################
# Necessary imports.                                          #
###############################################################
import re
###############################################################
# Short method to print data. (to text file)                  #
# Inputs: name - short string describing 'val'                #
#         val  - list of values pulled from xml               #
#         f    - the filehandle for the file to be printed to #
###############################################################
def print_data(name, val, f):
    if (name and val):
        #print ("\n%s\n    Value:  %s" % (name[0], val)) #Print to command line for debugging
        #f.write(str(name + ":"))
        f.write(str(name + ":::" + val))
        #for j in val:
        #    f.write(j + ",")
        f.write(";")
    #else:
        #print ("\nNo available %s from XML." % name[0])
###############################################################
# Parse xml and save data into array of [name, value] pairs   #
###############################################################
# Read teh xml
w_file = open("ndfd_info.xml", "r")
xml_str = w_file.read()
w_file.close()
# Open a file to print to
p_file = open("weather_data.txt", "w")
###
# Maximum temperature
###
max_t = re.search("<name>Daily Maximum Temperature</name>\s*<value>(\d*)</value>", xml_str)
if (max_t):
    print_data("Daily Maximum Temperature", max_t.group(1), p_file)
###
# Minimum temperature
###
min_t = re.search("<name>Daily Minimum Temperature</name>\s*<value>(\d*)</value>", xml_str)
if (min_t):
    print_data("Daily Minimum Temperature", min_t.group(1), p_file)
###
# 3 Hourly temperature
###
hrl_t = re.search("<name>Temperature</name>\s*<value>(\d*)</value>", xml_str)
if (hrl_t):
    print_data("Temperature", hrl_t.group(1), p_file)
###
# Apparent Temperature
###
app_t = re.search("<name>Apparent Temperature</name>\s*<value>(\d*)</value>", xml_str)
if (app_t):
    print_data("Apparent Temperature", app_t.group(1), p_file)
###
# Relative humidity
###
hum_r = re.search("<name>Relative Humidity</name>\s*<value>(\d*)</value>", xml_str)
if (hum_r):
    print_data("Relative Humidity", hum_r.group(1), p_file)
###
# Probability of precipitation for next 12 hours
###
prb_p = re.search("<name>12 Hourly Probability of Precipitation</name>\s*<value>(\d*)</value>", xml_str)
if (prb_p):
    print_data("12 Hourly Probability of Precipitation", prb_p.group(1), p_file)
###
# Liquid precipitation in inches
###
pcp_i = re.search("<name>Liquid Precipitation Amount</name>\s*<value>([\d\.]*)</value>", xml_str)
if (pcp_i):
    print_data("Liquid Precipitation Amount", pcp_i.group(1), p_file)
###
# Weather
###
wthr = re.search("<name>Weather Type, Coverage, and Intensity</name>\s*<weather-conditions>(\w*)</weather-conditions>", xml_str)
if (wthr):
    print_data("Weather Type, Coverage, and Intensity", wthr.group(1), p_file)
###
# Weather icons
###
icon = re.search('<name>Conditions Icons</name>\s*<icon-link>(https?://[^\s<>"]+|www\.[^s<>"]+)</icon-link>', xml_str)
if (icon):
    print_data("Conditions Icons", icon.group(1), p_file)
###
# Weather warnings and advisories
###
wwa = re.search("<name>Watches, Warnings, and Advisories</name>\s*<hazard-conditions>(\w*)</hazard-conditions>", xml_str)
if (wwa):
    print_data("Watches, Warnings, and Advisories", wwa.group(1), p_file)
p_file.close()