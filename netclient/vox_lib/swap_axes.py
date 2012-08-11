#!/usr/bin/python

"""
swap_axes.py

Swaps 2 axes, labelled x, y, or z
"""

import sys
import os.path

usage = """
Usage:
    swap_axes.py infile x y outfile
"""

if len(sys.argv) < 5:
    print usage
    sys.exit()
    
infile = sys.argv[1]
outfile = sys.argv[4]

axes = []
for i in range(2):
    a = sys.argv[i+2].lower()
    if a not in ['x', 'y', 'z']:
        print usage
        sys.exit()
    if a in axes:
        print usage
        sys.exit()
    axes.append(a)

if not os.path.exists(infile):
    print "File unknown: %s" % (infile,)
    print usage
    sys.exit()

def _swap(obj):
    new = obj[:]
    x,y,z = new
    if axes[0] == 'x':
        if axes[1] == 'y':
            new[0] = y
            new[1] = x
        else:
            new[0] = z
            new[2] = x
    elif axes[0] == 'y':
        if axes[1] == 'x':
            new[1] = x
            new[0] = y
        else:
            new[1] = z
            new[2] = y
    elif axes[0] == 'z':
        if axes[1] == 'x':
            new[2] = x
            new[0] = z
        else:
            new[2] = y
            new[1] = z
    return new

def swap_axes(f,g):
    lines = f.readlines()
    swapping = False
    dims = False
    for line in lines:
        line = line.strip()

        if 'dimension' in line:
            dims = True
            g.write(line + '\n')
            continue

        if dims:
            line = line.split(' ')
            line = _swap(map(int, line))
            g.write(' '.join(map(str,line)) + '\n')
            dims = False
            continue

        if 'voxels' in line:
            swapping = True
            g.write(line + '\n')
            continue

        if not swapping:
            g.write(line + '\n')
            continue

        line = map(int, line.split(' '))
        new = _swap(line[:3])
        line[:3] = new
        line = map(str, line)
        line = ' '.join(line)
        g.write(line + '\n')

def main():
    if infile == outfile:
        with open(infile, 'rw') as f:
            swap_axes(f, f)
    else:
        with open(infile, 'r') as f:
            with open(outfile, 'w') as g:
                swap_axes(f,g)

if __name__ == '__main__':
    main()



    

