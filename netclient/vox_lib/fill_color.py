#!/usr/bin/python

"""
Fill color

Takes 3 sets of arguments
--file <filename>
--base R,G,B    # color to replace
--fill R,G,B    # color to fill
"""

import argparse
import os
import shutil
import random

def get_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('file', type=str, help='input voxel model file')
    parser.add_argument('--base', nargs=3, type=int, help='base color to replace. R G B', required=True)
    parser.add_argument('--fill', nargs=3, type=int, help='color to replace with. R G B', required=True)
    args = parser.parse_args()
    return args

def check_infile(args):
    if not os.path.exists(args.file):
        print "File %s does not exist." % (args.file,)
        return False
    return True

def get_outfile(args):
    # get an unused filename to save backup to
    outfile = '%s.old' % (args.file)
    _outfile = outfile
    while  os.path.exists(outfile):
        outfile = _outfile + str(random.randrange(10000))
    return outfile

def process(args):
    # save old file
    shutil.copy2(args.file, get_outfile(args))

    # read file
    with open(args.file, 'r') as f:
        data = f.readlines()

    # split data into voxel lines and metadata
    metadata = []
    voxels = []
    found = False
    for line in data:
        if not line:
            continue
        if found:
            voxels.append(line.strip())
        else:
            metadata.append(line.strip())
        if 'voxels' in line:
            found = True

    # split into RGBA components
    voxels = [[int(c) for c in v.split()] for v in voxels]

    # splice in new RGBA
    for vox in voxels:
        if vox[-3:] == args.base:
            vox[-3:] = args.fill

    # reassemble string
    voxels = [map(str, v) for v in voxels]
    voxels = [' '.join(v) for v in voxels]

    # recombine metadata and voxel lines
    data = metadata + voxels

    # convert to string
    data += ['']
    data = '\n'.join(data)

    # write to file
    with open(args.file, 'w') as f:
        f.write(data)

def run():
    args = get_args()
    if not check_infile(args):
        return
    process(args)

if __name__ == '__main__':
    run()
