from PIL import Image
import sys, io, struct

im = Image.open('weathermap.bmp')

r, g, b = im.convert('RGB').split()
width, height = im.size

R = list(r.getdata())
G = list(g.getdata())
B = list(b.getdata())

L = R
for ind in range(len(R)):
    L[ind] = ((R[ind] & 0xF8) << 8) | ((G[ind] & 0xFC) << 3) | (B[ind] >> 3)

#outp = sys.stdout
outp = io.open('/dev/ttyACM0', 'wb')
#outp = io.open('test', 'wb')

# send bytes 0x0000 0x0000 (300 in hex) (275 in hex) then L
outp.write(struct.pack('<c', 'B'))
outp.write(struct.pack('<H', 100))
outp.write(struct.pack('<H', 0x0000))
outp.write(struct.pack('<H', width))
outp.write(struct.pack('<H', height))

for ind in range(len(L)):
#    struct.pack('<H', L[ind])
    outp.write(struct.pack('<H', L[ind]))
#    time.sleep(0.001)

outp.close()

