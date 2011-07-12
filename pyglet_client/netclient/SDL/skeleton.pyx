from skeleton_dat import *

cdef extern from 'skeleton_functions.h':
    int init6()
    int init7()
    int _draw_test()
    int _draw_test2()

def test():
    print "Init"
    #init6()
    init7()

def draw():
    _draw_test2()
    #_draw_test()
