#!/usr/bin/python

import json
import sys
import os

def get_args():
    infile = sys.argv[1]
    outfile = sys.argv[2]

    if not os.path.exists(infile) or not outfile:
        print """
            Usage:
            python json2vox.py <in.json> <out.dcvox>
            """
        sys.exit()

    if os.path.exists(outfile):
        overwrite = raw_input("%s exists. Overwrite? y/n" % (outfile,)).lower()
        while overwrite not in ['y', 'n']:
            overwrite = raw_input("%s exists. Overwrite? y/n" % (outfile,)).lower()
        if overwrite == 'n':
            print 'Abort'
            sys.exit()

    return infile, outfile
    
def convert(data):
    try:
        data = json.loads(data)
    except:
        print "Invalid json"
        sys.exit()

    args = []

    args += map(int, data['dim'])
    args += [float(data['vox_size'])]
    args += [int(data.get('biaxial', False))]
    args += [int(data.get('team', False))]
    args += map(int, data.get('team_base_color', [0,0,0]))

    print args
    
    new =\
"""#dimensions x,y,z
%d %d %d
#vox_size
%.2f
#biaxial
%d
#team
%d
#team_base_color (r,g,b)
%d %d %d
#voxels x,y,z, r,g,b
""" % tuple(args)

    for color in data['voxels']:
        new += "%d %d %d %d %d %d\n" % tuple(map(int, color[:6]))

    print new
    return new


if __name__ == '__main__':
    infile, outfile = get_args()
    with open(infile) as f:
        old = f.read()
    new = convert(old)
    with open(outfile, 'w') as f:
        f.write(new)
    print "Saved %s" % (outfile,)

        
    

