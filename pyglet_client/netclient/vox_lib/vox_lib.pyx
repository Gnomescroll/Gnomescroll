#from vox_dat import *

print "loaded"
cdef extern from 'vox_functions.h':
    int init7()
    int _draw_test2()

def test():
    print "Init"
    #init6()
    init7()

def draw():
    _draw_test2()
    #_draw_test()
