###############################################################
# Necessary imports. 'os' is used to grab file info, and      #
# 'time' is used to determine current time.
############################################################### 
import os
import time
###############################################################
# Get xml from the ndfd database, save it, parse the xml, and #
# save the information to a text file.                        #
###############################################################
# Only proceed with NDFD request if the last request is at
# least an hour old.  (The NDFD database only updates once an
# hour.  There is no reason to query it more often)
def update_weather_data():
    try:
        file_age_sec = (time.time() - os.stat('ndfd_info.xml').st_mtime)
    except:
        file_age_sec = 5000 # Default value >3600 if the file doesn't exist
    if (file_age_sec >= 3600):
        # Create a new xml document from the ndfd database
        os.system("python contact_ndfd_server.py")
        if not os.path.isfile('ndfd_info.xml'):
            exit()
        # Parse the xml document into a text file
        os.system("python parse_xml.py")
    # Parse the saved text file and return the parameters
    data_file = open("weather_data.txt", "r")
    actual_data = data_file.read()
    data_file.close()
    nv_dict = {}
    nv_pair_list = actual_data.split(';')
    for k in nv_pair_list:
        nv_pair = k.split(':::')
        if (nv_pair[0] and nv_pair[1]):
            nv_dict[nv_pair[0]] = nv_pair[1]
    # Download current weather imagej
    for j in nv_dict:
        if j == 'Conditions Icons':
            #print (str("wget " + nv_dict[j]))
            os.system(str("wget -o icon.jpg " + nv_dict[j]))
    # Now to return what we've found
    return nv_dict