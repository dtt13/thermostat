import os, time
from PIL import Image

TOPO_FILE = '/mnt/sda1/arduino/WeatherApp/topo.jpg'
NCR_FILE  = '/mnt/sda1/arduino/WeatherApp/ncr.gif'
WARN_FILE = '/mnt/sda1/arduino/WeatherApp/warn.gif'
CNTY_FILE = '/mnt/sda1/arduino/WeatherApp/county.gif'
CITY_FILE = '/mnt/sda1/arduino/WeatherApp/cities.gif'

MAP_FILE  = '/mnt/sda1/arduino/WeatherApp/map.jpg'

# Overlays map images, resizes and crops image to fit on the screen
def formatMap():
	if not os.path.isfile(MAP_FILE) or (time.time() - os.stat(MAP_FILE).st_mtime) > 3600
	downloadMapImages('cle') #TODO update to be dynamic
	topo = Image.open(TOPO_FILE)
	ncr = Image.open(NCR_FILE)
	warn = Image.open(WARN_FILE)
	cnty = Image.open(CNTY_FILE)
	city = Image.open(CITY_FILE)

	r, g, b, a_ncr = ncr.convert('RGBA').split()
	r, g, b, a_warn = warn.convert('RGBA').split()
	r, g, b, a_cnty = cnty.convert('RGBA').split()
	r, g, b, a_city = city.convert('RGBA').split()

	Image.composite(cnty, Image.composite(ncr, topo, a_ncr), a_cnty).convert('RGB').resize((300,275), Image.ANTIALIAS).crop((30, 30, 240, 212)).save(MAP_FILE)

# Downloads the map images to be combined together
def downloadMapImages(radarId):
	now = time.time()
	if not os.path.isfile(NCR_FILE) or (now - os.stat(NCR_FILE).st_mtime) > 3600:
		os.call(['wget', '-o', NCR_FILE, 'http://radar.weather.gov/ridge/RadarImg/NCR/' + radarId + '_NCR_0.gif'])
	if not os.path.isfile(WARN_FILE) or (now - os.stat(WARN_FILE).st_mtime) > 3600:
	os.call(['wget', '-o', WARN_FILE, 'http://radar.weather.gov/ridge/Warnings/Short/' + radarId + '_Short_Warnings_0.gif'])
	if not os.path.isfile(CITY_FILE):
		os.call(['wget', '-o', CITY_FILE, 'http://radar.weather.gov/ridge/Overlays/Cities/Short/' +  radarId + '_City_Short.gif'])
	if not os.path.isfile(TOPO_FILE):
		os.call(['wget', '-o', TOPO_FILE, 'http://radar.weather.gov/ridge/Overlays/Topo/Short/' + radarId + '_Topo_Short.jpg'])
	if not os.path.isfile(CNTY_FILE):
		os.call(['wget', '-o', CNTY_FILE, 'http://radar.weather.gov/ridge/Overlays/County/Short/' + radarId + '_County_Short.gif'])