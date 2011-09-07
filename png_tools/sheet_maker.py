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
parser.add_argument('-xc', '--extra-conf', default='')
parser.add_argument('--extra-conf-name', default='sprites')
parser.add_argument('-m', '--map-to', default='_spritemap.py', help='Generate python dict mapping subtexture locations from second spritesheet to first and save it to file')
parser.add_argument('-rm', '--reverse-map', action='store_true', help='Reverse mapping order. Default order is secondary location -> primary location')

args = parser.parse_args()


def zip_sprite_confs(s1, s2):
    global args
    s1 = dict(s1)
    d = {}
    fail = False
    for tex, i in s2:
        if tex not in s1:
            fail = 'WARNING: Constructing secondary spritesheet mapping, but secondary spritesheet contains textures not in primary spritesheet'
            print fail
            continue
        d[i] = s1[tex]

    if args.reverse_map:
        d = dict(zip(d.values(), d.keys()))
    
    return d, fail

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
        exec('from %s import %s as extra_sprites' % (conf, args.extra_conf_name,))
        spritesheet.Spritesheet(args.extra, args.source, extra_sprites).create()

        if args.map_to:
            mapping, fail_msg = zip_sprite_confs(sprites, extra_sprites)
            with open(args.map_to, 'w') as f:
                if fail_msg:
                    f.write('"""Generation of this file failed with message:\n%s"""\n\n' % (fail_msg,))
                f.write('_map=%s'% (mapping,))
            

if __name__ == '__main__':
    run()
