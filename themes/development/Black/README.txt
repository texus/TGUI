Requirements:
  - TexturePacker (https://www.codeandweb.com/texturepacker)

Input:
  - Files in images directory
  - Black.txt.in

Run the following command to generate Back.png and out.json from the input:
  TexturePacker images/ --format json --shape-padding 2 --sheet Black.png

Run generate.py with python to create Black.txt from the Black.png and Black.txt.in files.
