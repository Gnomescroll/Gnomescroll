# TIMER

cdef extern from "./time_lib/physics_timer.h":
    int _start_physics_timer(int frequency)
    int _tick_check()
    long _get_time()
    long _get_tick()
    void _START_CLOCK()
    int _GET_TICK()
    int _GET_MS_TIME()

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

def GET_MS_TIME():
    return _GET_MS_TIME()
