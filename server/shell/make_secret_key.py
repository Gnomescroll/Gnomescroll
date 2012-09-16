#!/usr/bin/env python

from binascii import hexlify
from os import urandom
from sys import argv, exit

USAGE = """
    %s [key_length=64] [output_file=stdout]
""" % (__name__,)

KEY_LENGTH = 64
OUTFILE = None

def make_key(keylen):
    return hexlify(urandom(keylen))

if __name__ == '__main__':

    key = make_key(KEY_LENGTH)

    assert len(key) == KEY_LENGTH * 2

    argc = len(argv)
    if argc > 1:
        try:
            KEY_LENGTH = int(argv[1])
        except ValueError:
            print USAGE
            exit()
        if argc > 2:
            OUTFILE = argv[2]

    if OUTFILE is None:
        print key
    else:
        with open(OUTFILE, 'w') as f:
            f.write(key)
        print "Wrote key to %s" % (OUTFILE,)
