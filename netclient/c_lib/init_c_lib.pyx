

cdef extern from "c_lib.c":
    int init_c_lib()

#print "Initing c_lib"

_init = 0
def init():
    global _init
    if _init == 0:
        init_c_lib()
    _init = 1

#network stuff

cdef extern from "./net_lib/client.c":
    void _NetClientTick()
    void _NetClientConnect(int a, int b,int c, int d, int _port)

def NetClientTick():
    _NetClientTick()

def NetClientConnect(int a, int b,int c, int d, int _port):
    _NetClientConnect(a,b,c,d, _port)
