#!/usr/bin/env python

# Run this Python program to download PART01 through PART08 from
# the same source I did, and unpack them into exactly the
# checked-in source files. The line counts below of course depend
# on the precise contents of the files PART01 through PART08.

import os

script = """
curl -O https://raw.githubusercontent.com/windowsdevops/Microsoft-Windows-NT-Repository/8e6359943f131f7a52a01108a01851bf46f0b6bf/COMPSRCS/GAMES/VMSNET/FUNADV/PART01
curl -O https://raw.githubusercontent.com/windowsdevops/Microsoft-Windows-NT-Repository/8e6359943f131f7a52a01108a01851bf46f0b6bf/COMPSRCS/GAMES/VMSNET/FUNADV/PART02
curl -O https://raw.githubusercontent.com/windowsdevops/Microsoft-Windows-NT-Repository/8e6359943f131f7a52a01108a01851bf46f0b6bf/COMPSRCS/GAMES/VMSNET/FUNADV/PART03
curl -O https://raw.githubusercontent.com/windowsdevops/Microsoft-Windows-NT-Repository/8e6359943f131f7a52a01108a01851bf46f0b6bf/COMPSRCS/GAMES/VMSNET/FUNADV/PART04
curl -O https://raw.githubusercontent.com/windowsdevops/Microsoft-Windows-NT-Repository/8e6359943f131f7a52a01108a01851bf46f0b6bf/COMPSRCS/GAMES/VMSNET/FUNADV/PART05
curl -O https://raw.githubusercontent.com/windowsdevops/Microsoft-Windows-NT-Repository/8e6359943f131f7a52a01108a01851bf46f0b6bf/COMPSRCS/GAMES/VMSNET/FUNADV/PART06
curl -O https://raw.githubusercontent.com/windowsdevops/Microsoft-Windows-NT-Repository/8e6359943f131f7a52a01108a01851bf46f0b6bf/COMPSRCS/GAMES/VMSNET/FUNADV/PART07
curl -O https://raw.githubusercontent.com/windowsdevops/Microsoft-Windows-NT-Repository/8e6359943f131f7a52a01108a01851bf46f0b6bf/COMPSRCS/GAMES/VMSNET/FUNADV/PART08
curl -O https://raw.githubusercontent.com/windowsdevops/Microsoft-Windows-NT-Repository/8e6359943f131f7a52a01108a01851bf46f0b6bf/COMPSRCS/GAMES/VMSNET/FUNADV/PATCH1
tail +10 PART01 | head -1069  >concatted.txt
tail +11 PART02 | head -1429 >>concatted.txt
tail +11 PART03 | head -761  >>concatted.txt
tail +11 PART04 | head -650  >>concatted.txt
tail +11 PART05 | head -1152 >>concatted.txt
tail +11 PART06 | head -1461 >>concatted.txt
tail +11 PART07 | head -1586 >>concatted.txt
tail +11 PART08              >>concatted.txt
"""

for line in script.splitlines():
  os.system(line.strip())

with open('concatted.txt', 'r') as f:
  lines = [line.rstrip('\n') for line in f.readlines()]

def output_it(current_file, current_line):
  if current_line is not None:
    for i in range(256):
      current_line = current_line.replace('`%02X' % i, chr(i))
    print(current_line, file=current_file)

def close_and_rename(current_file, filename):
  current_file.close()
  if filename == '$BUILD.COM':
    filename = 'BUILD.COM' # inauthentic but more shell-friendly
  os.rename('temp.txt', filename)

current_file = open('temp.txt', 'w')
current_line = None
for line in lines:
  if line.startswith('$ CALL UNPACK [.SRC]'):
    output_it(current_file, current_line)
    close_and_rename(current_file, line[20:].split(';')[0])
    current_file = open('temp.txt', 'w')
    current_line = None
  elif line.startswith('$ CALL UNPACK '):
    output_it(current_file, current_line)
    close_and_rename(current_file, line[14:].split(';')[0])
    current_file = open('temp.txt', 'w')
    current_line = None
  elif line.startswith('$'):
    pass
  elif line.startswith('X'):
    output_it(current_file, current_line)
    current_line = line[1:]
  elif line.startswith('V'):
    assert current_line is not None
    current_line += line[1:]
current_file.close()
os.unlink('temp.txt')
