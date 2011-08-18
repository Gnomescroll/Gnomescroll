#!/usr/bin/python

'''
Converts slab6 VOX format to gnomescroll's .vox json
'''

import argparse
import sys
import os.path
import json
import struct

class Parser:

    def __init__(self):
        self.args = self.parse()
        self.verify()
        
    def parse(self):

        p = argparse.ArgumentParser(description="Convert SLAB6 VOX files to DCMMO .vox json")

        p.add_argument('infile')
        p.add_argument('outfile')
        p.add_argument('-f', '--force', action='store_true')

        args = p.parse_args()
        return args

    def get_overwrite_command(self):
        return raw_input('Overwrite? [Y/n]').lower()

    def verify(self):
        p = self.args

        err = False
        if not os.path.exists(p.infile):
            print '%s file does not exist' % (p.infile,)
            err = True

        if not p.force:
            if os.path.exists(p.outfile):
                print '%s file exists' % (p.outfile,)
                while True:
                    over = self.get_overwrite_command()
                    if over in ['y', 'n']:
                        break
                if over == 'n':
                    print 'Aborting'
                    err = True
                else:
                    print 'Overwriting %s' % (p.outfile,)

        if err:
            sys.exit()
        

def convert(infile, outfile):

    with open(infile, 'r') as f:

        dims = f.read(12)
        xdim, ydim, zdim = struct.unpack('<3I', dims)
        f.seek(12)
        print xdim, ydim, zdim
        voxsize = xdim*ydim*zdim
        voxels = f.read(voxsize)
        voxfmt = '<%dB' % (voxsize,)
        voxels = struct.unpack(voxfmt, voxels)
        f.seek(voxsize)
        
        palette = f.read(768)
        palette = struct.unpack('<768B', palette)

    print xdim, ydim, zdim
    print voxels
    print palette
    
    d = {}

    with open(outfile, 'w') as f:
        json.dumps(d, f)
        
def main():
    p = Parser()
    convert(p.args.infile, p.args.outfile)

if __name__ == '__main__':
    main()
