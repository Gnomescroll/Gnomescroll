#!/usr/bin/python

'''
Converts v3a voxel format to dc's .vox json

'''

import json
from converter_args import Parser
        

def convert(infile, outfile):

    with open(infile) as f:
        lines = f.readlines()
        for line in lines:
            line = line.strip()
            if line.lower().startswith('size'):
                xdim, ydim, zdim = map(int, line.split(' ')[1:4])
                break
        del lines

    with open(infile) as f:
        voxels = []
        d = f.read().split('DATA')[1].strip()
        x = 0
        y = 0
        z = 0
        for line in d.split('\n'):
            z %= zdim
            y %= ydim
            line = line.strip()
            if not line:
                x += 1
                continue
            pts = line.split(' ')
            j = 0
            for pt in pts:
                if j%4 == 0:
                    vox = [int(pt)]
                    z += 1
                else:
                    vox.append(int(pt))
                    if len(vox) == 4 and vox != [-1,-1,-1,-1]:
                        vox = [x+1, y+1, z] + vox
                        voxels.append(vox)
                j += 1
            y += 1
            
    d = {
        'dim'   :   (xdim, ydim, zdim),
        'vosize':   0.2,
        'voxels':   voxels,
    }

    with open(outfile, 'w') as f:
        print 'writing to %s' % (outfile,)
        json.dump(d, f)

        
def main():
    p = Parser()
    convert(p.args.infile, p.args.outfile)

if __name__ == '__main__':
    main()
