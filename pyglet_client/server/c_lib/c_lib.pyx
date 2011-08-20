
cdef extern from "./physics_timer.h":
    int _start(int frequency)
    int _tick_check()
    long _get_time()
    long _get_tick()

def test():
    print "Test works"
    assert False

def start_physics_timer(int frequency):
    _start(frequency)

def tick_check():
    return _tick_check()

def get_time():
    return _get_time()

def get_tick():
    return _get_tick()

