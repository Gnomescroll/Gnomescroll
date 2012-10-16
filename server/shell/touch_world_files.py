#!/usr/bin/env python

import os

from get_version import get_version

def touch(fname, times=None):
    with file(fname, 'a'):
        os.utime(fname, times)

basepath = './world/'
bak = '.bak'
tmp = '.tmp'

filefmts = [ ('containers', 'ctr'),
             ('players', 'plr'),
             ('mechs', 'mch'),
             ('map', 'map') ]

fmt = '%s-%d.%s'

def run():
    version = get_version()

    for name, ext in filefmts:
        bp = os.path.join(basepath, name)
        fn = fmt % (name, version, ext)
        f = os.path.join(bp, fn)
        fbak = os.path.join(bp, fn + bak)
        if not os.path.exists(f) and not os.path.exists(fbak):
            touch(f)
    
if __name__ == '__main__':
    run()
