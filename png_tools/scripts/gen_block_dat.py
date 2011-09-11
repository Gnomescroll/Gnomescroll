#!/usr/bin/python

import sys
import os
sys.path.insert(0, './conf/')

from block_dat import dat


from block_sprite_conf import sprites

block_tex = {}

for tex, id in sprites:
    block_tex[tex] = id

print(str(block_tex))

import pprint



# 1> open configuration file
# parse file
# generate dictionary for cubes
# write dictionary to cube_dat

f = open("../server/dats/cube_dat.py", "w")
pp = pprint.PrettyPrinter(stream=f, indent=4)

f.write("dat = ")
pp.pprint(dat)
