#!/usr/bin/python

from converter_args import Parser

import subprocess

'''
Converts Ken Silverman's vox format to DC's json vox
via Ken Silverman's vox2v3a tool, and DC's v3a2dcvox.py
'''

def convert(infile, outfile):

    # run vox2v3a.o
    #run v3a2dcvox.py
    tmpf = '%s.tmp' % (outfile,)
    args = ['./slab6vox2v3a.o', infile, tmpf]
    subprocess.Popen(args)

    args = ['python', 'v3a2dcvox.py', tmpf, outfile, '-f']
    subprocess.Popen(args)

    args = ['rm', '-f', tmpf]
    subprocess.Popen(args)

    print 'Done'
    

if __name__ == '__main__':
    args = Parser().args
    infile, outfile = args.infile, args.outfile
    convert(infile, outfile)
