#!/usr/bin/python

import spritesheet
import particles_sprite_conf as sprites

texture_path = './block/'
name = 'blocks'

if __name__ == '__main__':
    spritesheet.Spritesheet(name, texture_path, sprites).create()
