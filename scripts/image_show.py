#!/usr/bin/env python

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
    
    imagergb = Image.new(mode = "RGB", size = (width, height))
    for y in range(height):
        for x in range(width):
            r = int.from_bytes(f.read(1), byteorder='little')
            g = int.from_bytes(f.read(1), byteorder='little')
            b = int.from_bytes(f.read(1), byteorder='little')
            imagergb.putpixel((x,y),(r,g,b))

    print(os.path.splitext(sys.argv[1])[0] + '.png')
    imagergb.save(os.path.splitext(sys.argv[1])[0] + '.png', "PNG")
