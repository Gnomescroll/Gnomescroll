#cdef extern from "c_lib.hpp":

cdef extern from "c_lib.hpp":
    int init_c_lib()

#print "Initing c_lib"

_init = 0
def init():
    global _init
    if _init == 0:
        init_c_lib()
    _init = 1

## net stuff

#cdef extern from "./net_lib/server.h":
cdef extern void _NetServerInit()
cdef extern void _NetServerTick()

def NetServerInit():
    _NetServerInit()

def NetServerTick():
    _NetServerTick()

##timer

cdef extern from "../c_lib/time/physics_timer.h":
    int _start_physics_timer(int frequency)
    int _tick_check()
    long _get_time()
    long _get_tick()
    void _START_CLOCK()
    int _GET_TICK()

 #DEPRECATE
def StartPhysicsTimer(frequency):
    _start_physics_timer(frequency)

#DEPRECATE
def PhysicsTimerTickCheck():
    return _tick_check()

 #DEPRECATE
def get_time():
    return _get_time()

 #DEPRECATE
def get_tick():
    return _get_tick()

def START_CLOCK():
    _START_CLOCK()

def GET_TICK():
    return _GET_TICK()
