import os
import json
from string import Template

mapping = {}
with open('out.json', 'r') as f:
    data = json.load(f)
    for imageFilename in data['frames']:
        frame = data['frames'][imageFilename]['frame']
        filenameWithoutExtension = os.path.splitext(imageFilename)[0]
        mapping[filenameWithoutExtension] = '"Black.png" Part({}, {}, {}, {})'.format(
            frame['x'], frame['y'], frame['w'], frame['h'])

with open('Black.txt.in', 'r') as f:
  contents = f.read()

changedContents = Template(contents).substitute(mapping)
with open('Black.txt', 'w') as f:
    f.write(changedContents)
