

cdef extern from "c_lib.hpp":
    int init_c_lib()
    void close_c_lib()

#print "Initing c_lib"

_init = 0
def init():
    global _init
    if _init == 0:
        init_c_lib()
    _init = 1

def close():
    close_c_lib()

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
'''
cdef extern from "../c_lib/time/physics_timer.h":
    int _start_physics_timer(int frequency)
    int _tick_check()
    long _get_time()
    long _get_tick()
'''

#new functions
cdef extern from "../c_lib/time/physics_timer.h":
    void _START_CLOCK()
    int _GET_TICK()
    int _GET_MS_TIME()


 #DEPRECATE
def StartPhysicsTimer(frequency):
    #_start_physics_timer(frequency)
    _START_CLOCK()

#DEPRECATE
def PhysicsTimerTickCheck():
    return _GET_TICK()
    #return _tick_check()

 #DEPRECATE
def get_time():
    return _GET_MS_TIME();
    #return _get_time()

 #DEPRECATE
def get_tick():
    return _GET_TICK()
#    return _get_tick()

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
    ctypedef void (*PY_CLIENT_EVENT_CALLBACK)(int client_id, int event_type)
    void set_python_net_callback_function(PY_MESSAGE_CALLBACK pt)
    void set_python_net_event_callback_function(PY_CLIENT_EVENT_CALLBACK pt)
    void send_python_net_message(char* message, int size, int client_id)
    int _get_client_id()
    int _check_connection_status()

_CLIENT_CREATION_CALLBACK = None
_CLIENT_DELETION_CALLBACK = None
_CLIENT_MESSAGE_CALLBACK = None

def register_client_creation(function):
    global _CLIENT_CREATION_CALLBACK
    _CLIENT_CREATION_CALLBACK = function

def register_client_deletion(function):
    global _CLIENT_DELETION_CALLBACK
    _CLIENT_DELETION_CALLBACK = function

def register_client_message_handling(function):
    global _CLIENT_MESSAGE_CALLBACK
    _CLIENT_MESSAGE_CALLBACK = function

def get_client_id():
    return _get_client_id()

def connected():
    return _check_connection_status()

cdef void py_net_message_callback(char* buff, int n, int client_id):
    ustring = buff[:n]
    if(_CLIENT_MESSAGE_CALLBACK != None):
        _CLIENT_MESSAGE_CALLBACK(client_id, ustring)
'''
    cdef bytes py_string = buff[:n]
    if(_CLIENT_MESSAGE_CALLBACK != None):
        _CLIENT_MESSAGE_CALLBACK(client_id, bytes)
'''

'''
    ustring = buff[:n].decode('UTF-8')
    if(_CLIENT_MESSAGE_CALLBACK != None):
        _CLIENT_MESSAGE_CALLBACK(client_id, ustring)
    #print "python callback: received %i bytes from client %i" % (n, client_id)
    #print "str: %s" % (ustring)
'''

def _send_python_net_message(message, int client_id):
    #print "Send python net message"
    cdef int length = len(message)
    cdef char* c_string = message
    send_python_net_message(message, length, client_id)

cdef void py_net_net_event_callback(int client_id, int event_type):
    if event_type == 0:
        print "Client connected: %i" % (client_id)
        _CLIENT_CREATION_CALLBACK(client_id)
    if event_type == 1:
        print "Client disconnected: %i" % (client_id)
        print "Client disconnected: %i" % (client_id)

cpdef init_python_net():
    cdef PY_MESSAGE_CALLBACK p = py_net_message_callback
    set_python_net_callback_function(py_net_message_callback)
    print "Python net callback set"
    set_python_net_event_callback_function(py_net_net_event_callback)