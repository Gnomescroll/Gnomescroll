#!/usr/bin/python

import argparse
import spritesheet
import sys

parser = argparse.ArgumentParser()
parser.add_argument('name', help='Name of spritesheet to be created')
parser.add_argument('source', help='Location of source textures')
parser.add_argument('conf', help='Configuration file for spritesheet')
parser.add_argument('--conf_name', default='sprites', help='Name of sprite configuration list within the spritesheet configuration module')

parser.add_argument('-x', '--extra', default='', help='Name of additional spritesheet to generate (such as block_selector hud texture)')
parser.add_argument('-xc', '--extra_conf', default='')
parser.add_argument('--extra_conf_name', default='sprites')

args = parser.parse_args()

def run():
    global args

    conf = args.conf.split('.')[0]
    exec('from %s import %s as sprites' % (conf, args.conf_name,))
    spritesheet.Spritesheet(args.name, args.source, sprites).create()

    if args.extra:
        if not args.extra_conf:
            print 'Requested generation of extra spritesheet %s, but no extra conf was given. Use -xc or --extra_conf to specify'
            sys.exit()
        conf = args.extra_conf.split('.')[0]
        exec('from %s import %s as sprites' % (conf, args.extra_conf_name,))
        spritesheet.Spritesheet(args.extra, args.source, sprites)

if __name__ == '__main__':
    run()
