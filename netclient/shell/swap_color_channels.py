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
    parser.add_argument('channel_map', help='Destination RGB channel. e.g RGB,BGR,BRG')
    parser.add_argument('--out', dest='output_image', help='output image')
    args = parser.parse_args()

    if args.channel_map == 'all':
        rgb = list('RGB')
        args.channel_map = itertools.permutations(rgb)
        args.channel_map = [''.join(list(rgb) + ['A']) for rgb in args.channel_map]
    else:
        if len(args.channel_map) == 3:
            args.channel_map += 'A'
        args.channel_map = [args.channel_map]

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
    new_im = Image.new('RGBA', im.size, (0, 0, 0, 0))
    
    swap_maps = [get_swap_map(rgb) for rgb in args.channel_map]

    for n, m in enumerate(swap_maps):    
        for i in range(im.size[0]):
            for j in range(im.size[1]):
                pix = im.getpixel((i,j))
                swap_pix = [pix[p] for p in m]
                new_im.putpixel((i,j), tuple(swap_pix))

        out = args.output_image
        if not out:
            pieces = args.image.split('.')
            out = "%s-%s.%s" % (pieces[0], args.channel_map[n], pieces[1],)
            
        print "Saving", out
        new_im.save(out)

if __name__ == "__main__":
    args = get_args()
    run(args)
