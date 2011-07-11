from skeleton_dat import *

cdef extern from 'skeleton_functions.h':
    int init6()
    int _draw_test()

def init():
    print "Init"
    init6()

def draw():
    _draw_test()
