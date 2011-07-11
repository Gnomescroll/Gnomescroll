from skeleton_dat import *

extern from 'skeleton_functions.c':
    int init6()

def init():
    print "Init"
    init6()
