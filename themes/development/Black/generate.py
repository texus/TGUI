import os
import json
import numpy as np
from string import Template
from PIL import Image
from PyTexturePacker import Packer

# Extrude seems to be required to properly render stretched images with smoothing.
# Without it, stretching images (e.g. scrollbar track) will have semi-transparent edges (or the
# color of the next image if we wouldn't be adding transparent padding).
# The reduce_border_artifacts option does not solve this.
# Marking the problematic images as NoSmooth in the theme file would also solve this issue though.
extrudePixels = True

packer = Packer.create(
    bg_color=0x00ffffff,
    texture_format='.png',
    max_width=4096,
    max_height=4096,
    enable_rotated=False,
    force_square=False,
    border_padding = 0 if extrudePixels else 1,
    shape_padding = 0 if extrudePixels else 2,
    reduce_border_artifacts = not extrudePixels,  # Causes pixels to have alpha=1 which don't need it
    extrude=extrudePixels,
    atlas_format='json')

packer.pack('images/', 'tmp')

# Shrink the image as it doesn't has to have a width and height that are a power of 2
pixels = np.array(Image.open('tmp.png').convert('RGBA'))
inImgHeight = len(pixels)
inImgWidth = len(pixels[0])

requiredRows = inImgHeight
for y in range(inImgHeight, 0, -1):
    requiredRowFound = False
    for x in range(inImgWidth):
        if pixels[y-1][x][3] != 0:
            requiredRowFound = True
            requiredRows = y
            break
    if requiredRowFound:
        break

requiredColumns = inImgWidth
for x in range(inImgWidth, 0, -1):
    requiredColumnsFound = False
    for y in range(inImgHeight):
        if pixels[y][x-1][3] != 0:
            requiredColumnsFound = True
            requiredColumns = x
            break
    if requiredColumnsFound:
        break

Image.fromarray(pixels[0:requiredRows,0:requiredColumns]).save('Black.png')

# Generate the theme file based on the template and where the texture packer placed each image
mapping = {}
with open('tmp.json', 'r') as f:
    data = json.load(f)
    for imageFilename in data['frames']:
        frame = data['frames'][imageFilename]['frame']
        filenameWithoutExtension = os.path.splitext(imageFilename)[0]
        posX = frame['x']+1 if extrudePixels else frame['x']  # Extrude option in texture packer seems broken
        posY = frame['y']+1 if extrudePixels else frame['y']  # Extrude option in texture packer seems broken
        mapping[filenameWithoutExtension] = '"Black.png" Part({}, {}, {}, {})'.format(
            posX, posY, frame['w'], frame['h'])

with open('Black.txt.in', 'r') as f:
    contents = f.read()

changedContents = Template(contents).substitute(mapping)
with open('Black.txt', 'w') as f:
    f.write(changedContents)

# Remove teporary files created by texture packer
os.remove('tmp.json')
os.remove('tmp.png')
