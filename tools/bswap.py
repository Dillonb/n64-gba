#!/usr/bin/env python3
import sys

if len(sys.argv) != 3:
    print("usage: %s <infile> <outfile>" % sys.argv[0])
    quit(1)

f = open(sys.argv[1], "rb")
content = f.read()
f.close()

swapped = []

for i in range(0, len(content) // 4):
    base = i * 4
    swapped.append(content[base + 3])
    swapped.append(content[base + 2])
    swapped.append(content[base + 1])
    swapped.append(content[base + 0])

f = open(sys.argv[2], "wb")
f.write(bytearray(swapped))
f.close()