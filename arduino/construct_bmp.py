# convert a .png image file to a .bmp image file using PIL

from PIL import Image


topo = Image.open("topo.jpg")
ncr = Image.open("ncr.gif")
warn = Image.open("warn.gif")
cnty = Image.open("county.gif")
city = Image.open("cities.gif")

r, g, b, a_ncr = ncr.convert('RGBA').split()
r, g, b, a_warn = warn.convert('RGBA').split()
r, g, b, a_cnty = cnty.convert('RGBA').split()
r, g, b, a_city = city.convert('RGBA').split()

# Need to add county lines better
Image.composite(cnty, Image.composite(ncr, topo, a_ncr), a_cnty).convert('RGB').resize((300,275), Image.ANTIALIAS).save("weathermap.bmp")
#Image.composite(city, Image.composite(cnty, Image.composite(ncr, topo, a_ncr), a_cnty), a_city).convert('RGB').resize((300,275), Image.ANTIALIAS).save("weathermap.bmp")
