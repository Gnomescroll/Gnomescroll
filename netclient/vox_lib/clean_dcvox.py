#!/usr/bin/python

"""
Clean dcvox

Takes 2 arguments (1 optional)
file <filename>
[out] <filename>
"""

import argparse
import os
import shutil
import random

def get_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('files', type=str, nargs='*', help='input voxel model file')
    parser.add_argument('--outfile', type=str, help='file to save old version to', default='')
    args = parser.parse_args()
    return args

def check_infile(args):
    if not os.path.exists(args.file):
        print "File %s does not exist." % (args.file,)
        return False
    return True

def get_outfile(args):
    # get an unused filename to save backup to
    if args.outfile:
        return args.outfile
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
    voxels_found = False
    dimensions_found = False
    dimensions_found_at = None
    for i, line in enumerate(data):
        line = line.strip()
        if not line:
            continue
        if dimensions_found and dimensions_found_at is None:
            dimensions_found_at = i
        if voxels_found:
            voxels.append(line)
        else:
            metadata.append(line)
        if 'voxels' in line:
            voxels_found = True
        if 'dimension' in line:
            dimensions_found = True

    if dimensions_found_at is None:
        raise ValueError, "No dimensions property found in %s. Invalid format. Abort" % (args.file,)

    # split into XYZ-RGBA components
    voxels = [[int(c) for c in v.split()] for v in voxels]

    # determine dimension boundaries
    minx = miny = minz = 999999999999
    maxx = maxy = maxz = 0
    for v in voxels:
        x,y,z = v[:3]

        if x < minx:
            minx = x
        if y < miny:
            miny = y
        if z < minz:
            minz = z
            
        if x > maxx:
            maxx = x
        if y > maxy:
            maxy = y
        if z > maxz:
            maxz = z

    print "Min: %d,%d,%d" % (minx, miny, minz)
    print "Max: %d,%d,%d" % (maxx, maxy, maxz)

    # adjust voxel coordinates to be 0-indexed
    for v in voxels:
        v[0] -= minx
        v[1] -= miny
        v[2] -= minz

    # adjust dimensions to be 0-indexed
    maxx -= minz
    maxy -= miny
    maxz -= minz
    maxx += 1
    maxy += 1
    maxz += 1
    minx = 0
    miny = 0
    minz = 0

    print "Max(adj): %d,%d,%d" % (maxx, maxy, maxz)

    # reassemble voxels string
    voxels = [map(str, v) for v in voxels]
    voxels = [' '.join(v) for v in voxels]

    # update dimensions metadata
    dimensions = [maxx, maxy, maxz]
    dimensions = map(str, dimensions)
    dimensions = ' '.join(dimensions)
    metadata[dimensions_found_at] = dimensions

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
    for f in args.files:
        args.file = f
        if not check_infile(args):
            continue
        try:
            process(args)
        except Exception, e:
            print "Failed to process %s" % (f,)
            print Exception, e
            continue

if __name__ == '__main__':
    run()
