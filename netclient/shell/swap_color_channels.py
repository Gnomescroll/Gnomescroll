#!/usr/bin/env python

"""
Swap RGB color channels in an image according to mapping

Usage:
    script.py input_image output_image MAP
"""

import argparse
import itertools
import Image

def get_args():
    parser = argparse.ArgumentParser(description='Swap RGB channels')
    parser.add_argument('image', help='input image')
    parser.add_argument('output_image', help='output image')
    parser.add_argument('channel_map', help='Destination RGB channel. e.g RGB,BGR,BRG')
    args = parser.parse_args()

    if len(args.channel_map) == 3:
        args.channel_map += 'A'
    
    return args

def get_swap_map(channels):
    SWAP_MAP = {
        'R': 0,
        'G': 1,
        'B': 2,
        'A': 3
        }

    channels = list(channels)
    channels = [SWAP_MAP[c.upper()] for c in channels]
    return channels

def run(args):

    im = Image.open(args.image)
    swap_map = get_swap_map(args.channel_map)

    #for i,j in itertools.product(range(im.size[0]), range(im.size[1])):
        #pix = im.getpixel((i,j))
        #swap_pix = [pix[p] for p in swap_map]
        #im.putpixel((i,j), tuple(swap_pix))
        
    for i in range(im.size[0]):
        for j in range(im.size[1]):
            pix = im.getpixel((i,j))
            swap_pix = [pix[p] for p in swap_map]
            im.putpixel((i,j), tuple(swap_pix))

    im.save(args.output_image)

if __name__ == "__main__":
    args = get_args()
    run(args)
