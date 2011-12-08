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

cdef extern from "./net_lib/server.h":
    cdef extern void _NetServerInit()
    cdef extern void _NetServerTick()

def NetServerInit():
    _NetServerInit()

def NetServerTick():
    _NetServerTick()

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

#consider deprecating?

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




### python network

cdef extern from "./net_lib/export.hpp":
    ctypedef void (*PY_MESSAGE_CALLBACK)(char* buff, int n, int client_id)
    void set_python_net_callback_function(PY_MESSAGE_CALLBACK pt)
    void send_python_net_message(char* message, int size, int client_id)
    int _check_connection_status(int client_id)

def connected(client_id):
    return _check_connection_status(client_id)

cdef void py_net_callback(char* buff, int n, int client_id):
    ustring = buff[:n].decode('UTF-8')
    #print "str: %s" % (ustring)
    print "%i bytes,from client %i: %s" % (n, client_id, ustring)


cpdef init_python_net():
    cdef PY_MESSAGE_CALLBACK p = py_net_callback
    set_python_net_callback_function(py_net_callback)
    print "Python net callback set"

def _send_python_net_message(message, int client_id):
    print "Send python net message"
    cdef int length = len(message)
    cdef char* c_string = message
    send_python_net_message(message, length, client_id)