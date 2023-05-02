from PIL import Image
import os
import sys

with open(sys.argv[1], 'rb') as f:
    magic = f.read(5).decode('utf-8')
    if magic != "AFB:D":
        print("Not a valid AFB file!")
        os.exit()
    width = int.from_bytes(f.read(4), byteorder='little')
    height = int.from_bytes(f.read(4), byteorder='little')
    r = 0
    g = 0
    b = 0
    
    # imager = Image.new(mode = "RGB", size = (width, height))
    # imageg = Image.new(mode = "RGB", size = (width, height))
    # imageb = Image.new(mode = "RGB", size = (width, height))
    imagergb = Image.new(mode = "RGB", size = (width, height))
    for y in range(height):
        for x in range(width):
            r = int.from_bytes(f.read(1), byteorder='little')
            g = int.from_bytes(f.read(1), byteorder='little')
            b = int.from_bytes(f.read(1), byteorder='little')
            # imager.putpixel((x,y),(r,0,0))
            # imageg.putpixel((x,y),(0,g,0))
            # imageb.putpixel((x,y),(0,0,b))
            imagergb.putpixel((x,y),(r,g,b))
    # imager.show()
    # imageg.show()
    # imageb.show()
    # imagergb.show()
    imagergb.save(os.path.splitext(sys.argv[1])[0]+'.png',"PNG")
