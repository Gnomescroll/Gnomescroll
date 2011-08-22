

cdef extern from "c_lib.c":
    int init_c_lib()

print "Initing c_lib"

def init():
    init_c_lib()

init()
