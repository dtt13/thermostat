###############################################################
# Necessary imports.                                          #
###############################################################
import xml.etree.ElementTree as ET
#from lxml import etree
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
# NDFD xml follows a strict schema, so we can use xpath to    #
# directly search for the desired result.                     #
###############################################################
#Make the parser parse the xml document into an ETree object.
doc_tree = ET.parse('ndfd_info.xml')
doc_root = doc_tree.getroot()
# Open a file to print to
p_file = open("weather_data.txt", "w")
# Maximum temperature
max_t_node = doc_root.find(".//parameters/temperature[@type='maximum']")
if max_t_node is not None:
    max_t_name = (max_t_node.find('name').text)
    max_t_value = (max_t_node.find('value').text)
    if (max_t_name and max_t_value):
        print_data(max_t_name, max_t_value, p_file)
# Minimum temperature
min_t_node = doc_root.find(".//parameters/temperature[@type='minimum']")
if min_t_node is not None:
    min_t_name = (min_t_node.find('name').text) 
    min_t_value = (min_t_node.find('value').text)
    if (min_t_name and min_t_value):
        print_data(min_t_name, min_t_value, p_file)
# 3 Hourly temperature
hrl_t_node = doc_root.find(".//parameters/temperature[@type='hourly']")
if hrl_t_node is not None:
    hrl_t_name = (hrl_t_node.find('name').text)
    hrl_t_value = (hrl_t_node.find('value').text)
    if (hrl_t_name and hrl_t_value):
        print_data(hrl_t_name, hrl_t_value, p_file)
# Apparent Temperature
app_t_node = doc_root.find(".//parameters/temperature[@type='apparent']")
if app_t_node is not None:
    app_t_name = (app_t_node.find('name').text)
    app_t_value = (app_t_node.find('value').text)
    if (app_t_name and app_t_value):
        print_data(app_t_name, app_t_value, p_file)
# Relative humidity
hum_r_node = doc_root.find(".//parameters/humidity[@type='relative']")
if hum_r_node is not None:
    hum_r_name = (hum_r_node.find('name').text)
    hum_r_value = (hum_r_node.find('value').text)
    if (hum_r_name and hum_r_value):
        print_data(hum_r_name, hum_r_value, p_file)
# Probability of precipitation for next 12 hours
prb_p_node = doc_root.find(".//parameters/probability-of-precipitation[@type='12 hour']")
if prb_p_node is not None:
    prb_p_name = (prb_p_node.find('name').text)
    prb_p_value = (prb_p_node.find('value').text)
    if (prb_p_name and prb_p_value):
        print_data(prb_p_name, prb_p_value, p_file)
# Liquid precipitation in inches
pcp_i_node = doc_root.find(".//parameters/precipitation[@type='liquid']")
if pcp_i_node is not None:
    pcp_i_name = (pcp_i_node.find('name').text)
    pcp_i_value = (pcp_i_node.find('value').text)
    if (pcp_i_name and pcp_i_value):
        print_data(pcp_i_name, pcp_i_value, p_file)
# Weather
wthr_node = doc_root.find(".//parameters/weather")
if wthr_node is not None:
    wthr_name = (wthr_node.find('name').text)
    wthr_cond = (wthr_node.find('weather-conditions').text)
    if (wthr_name and wthr_cond):
        print_data(wthr_name, wthr_cond, p_file)
# Weather icons
icon_node = doc_root.find(".//parameters/conditions-icon[@type='forecast-NWS']")
if icon_node is not None:
    icon_name = (icon_node.find('name').text)
    icon_link = (icon_node.find('icon-link').text)
    if (icon_name and icon_link):
        print_data(icon_name, icon_link, p_file)
# Weather warnings and advisories
wwa_node = doc_root.find(".//parameters/hazards")
if wwa_node is not None:
    wwa_name = (wwa_node.find('name').text)
    wwa_cond = (wwa_node.find('hazard-conditions').text)
    if (wwa_name and wwa_cond):
        print_data(wwa_name, wwa_cond, p_file)
# Close the file we were printing to
p_file.close()