

cdef extern from "c_lib.c":
    int init_c_lib()

#print "Initing c_lib"

_init = 0
def init():
    global _init
    if _init == 0:
        init_c_lib()
    _init = 1

## net stuff

cdef extern from "./net_lib/server.h":
    void _NetServerInit()
    void _NetServerTick()

def NetServerInit():
    _NetServerInit()

def NetServerTick():
    _NetServerTick
