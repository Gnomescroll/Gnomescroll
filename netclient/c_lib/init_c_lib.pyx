

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

cdef extern from "./net_lib/client.h":
    void _NetClientTick()
    void _NetClientConnect(int a, int b,int c, int d, int _port)

def NetClientTick():
    _NetClientTick()

def NetClientConnect(int a, int b,int c, int d, int _port):
    _NetClientConnect(a,b,c,d, _port)


##timer

cdef extern from "../c_lib/time/physics_timer.h":
    int _start(int frequency)
    int _tick_check()
    long _get_time()
    long _get_tick()

def StartPhysicsTimer(frequency):
    _start(frequency)

def PhysicsTimerTickCheck():
    return _tick_check()

def get_time():
    return _get_time()

def get_tick():
    return _get_tick()
