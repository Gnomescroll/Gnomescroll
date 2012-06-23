#!/usr/bin/python

'''
Converts slab6 VOX format to gnomescroll's .vox json

NOT COMPLETE
'''

import json
import struct

from converter_args import Parser
        

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
