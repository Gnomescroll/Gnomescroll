

cdef extern from "c_lib.hpp":
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
    void _NetClientStartFrame()
    void _NetClientStateTick()
    void _NetClientNetInTick()
    void _NetClientNetOutTick()
    void _NetClientConnect(int a, int b,int c, int d, int _port)
#   void _NetClientTick()

#def NetClientTick():
#    _NetClientTick()

def NetClientStateTick():
    _NetClientStateTick()

def NetClientNetInTick():
    _NetClientNetInTick()

def NetClientNetOutTick():
    _NetClientNetOutTick()

### The problem
def NetClientStartFrame():
    _NetClientStartFrame()

def NetClientConnect(int a, int b,int c, int d, int _port):
    _NetClientConnect(a,b,c,d, _port)

##timer

#old functions: deprecate
cdef extern from "../c_lib/time/physics_timer.h":
    int _start_physics_timer(int frequency)
    int _tick_check()
    long _get_time()
    long _get_tick()

#new functions
cdef extern from "../c_lib/time/physics_timer.h":
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
    return _GET_MS_TIME();

### pviz

cdef extern from "./net_lib/client/pviz.h":
    void pviz_draw(float x, float y, float z)
    void toggle_latency_unit()

def _pviz_draw(float x, float y, float z):
    pviz_draw(x,y,z)

def _toggle_latency_unit():
    toggle_latency_unit()


### python network

cdef extern from "./net_lib/export.hpp":
    ctypedef void (*PY_MESSAGE_CALLBACK)(char* buff, int n, int client_id)
    void set_python_net_callback_function(PY_MESSAGE_CALLBACK pt)

cdef void py_net_callback(char* buff, int n, int client_id):
    print "python callback: received %i bytes from client %i" % (n, client_id)

cpdef init_python_net():
    cdef PY_MESSAGE_CALLBACK p = py_net_callback
    set_python_net_callback_function(py_net_callback)
    print "Python net callback set"