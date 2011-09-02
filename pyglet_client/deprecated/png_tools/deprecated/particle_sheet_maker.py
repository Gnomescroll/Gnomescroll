#!/usr/bin/python

import spritesheet
from particles_sprite_conf import sprites

texture_path = './particle/'
name = 'particles'

if __name__ == '__main__':
    spritesheet.Spritesheet(name, texture_path, sprites).create()
