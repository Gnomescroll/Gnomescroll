#!/usr/bin/python

'''
Argument parser for infile/outfile for converters
'''

import argparse
import sys
import os.path

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
