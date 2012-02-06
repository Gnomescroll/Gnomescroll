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

cdef extern from "./net_lib/host.hpp":
    void init_net_server(int a, int b, int c, int d, int port)
    void server_dispatch_network_events()
    void flush_to_net()

def NetServerInit(int a=0, int b=0, int c=0, int d=0, int port=0):
    init_net_server(a,b,c,d,port)

def NetServerDispatchNetworkEvents():
    server_dispatch_network_events()

def NetServerFlushToNet():
    flush_to_net()

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
    ctypedef void (*PY_CLIENT_EVENT_CALLBACK)(int client_id, int event_type)
    void set_python_net_callback_function(PY_MESSAGE_CALLBACK pt)
    void set_python_net_event_callback_function(PY_CLIENT_EVENT_CALLBACK pt)
    void send_python_net_message(char* message, int size, int client_id)
    int _check_connection_status(int client_id)

def connected(client_id):
    return _check_connection_status(client_id)

def _send_python_net_message(message, int client_id):
    #print "Send python net message"
    cdef int length = len(message)
    cdef char* c_string = message
    send_python_net_message(message, length, client_id)

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

_total_python_bytes = 0 
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
    #_total_python_bytes += n
    #print "%i bytes,from client %i: %s" % (n, client_id, ustring)
    if(_CLIENT_MESSAGE_CALLBACK != None):
        _CLIENT_MESSAGE_CALLBACK(client_id, ustring)
'''

cdef void py_net_event_callback(int client_id, int event_type):
    if event_type == 0:
        print "Client connected: %i" % (client_id)
        _CLIENT_CREATION_CALLBACK(client_id)
    if event_type == 1:
        print "Client disconnected: %i" % (client_id)
        _CLIENT_DELETION_CALLBACK(client_id)

cpdef init_python_net():
    pass
    #cdef PY_MESSAGE_CALLBACK p = py_net_message_callback
    set_python_net_callback_function(py_net_message_callback)
    print "Python net callback set"
    set_python_net_event_callback_function(py_net_event_callback)

#register_client_creation, register_client_deletion, register_client_message_handling


cdef extern from "c_lib.hpp":
    void _set_seed(int seed)
    
def set_seed(int seed):
    _set_seed(seed)

cdef extern from "./state/server_state.hpp" namespace "ServerState":
    void server_tick()
    void send_game_state_to_client(int client_id)

def tick_server_state():
    server_tick();

def send_game_state(int client_id):
    send_game_state_to_client(client_id)

"""
    Condensed cython files here
"""

""" Monsters """
cdef extern from "./monsters/monsters.hpp" namespace "Monsters":
    cdef cppclass Slime_list:
        void tick()
    void test(int n)

cdef extern from "./state/server_state.hpp" namespace "ServerState":
    Slime_list slime_list

def slime_test(int n):
    test(n)

def slime_tick():
    slime_list.tick()

""" Agents """
'''
Wrapper
'''

#agent class wrapper

from libcpp cimport bool
#AgentState
cdef extern from "./agent/agent.hpp":
    cdef cppclass AgentState:
        int seq
        float theta
        float phi
        float x,y,z
        float vx,vy,vz
        #bool crouching
        bool jump_ready

cdef extern from "./agent/agent_status.hpp":
    unsigned int PLAYER_NAME_MAX_LENGTH
    cdef cppclass Agent_status:
        int health
        bool dead
        int team
        int score()
        int kills
        int deaths
        int suicides
        char* name
        bool identified
#        void set_name(char* name)
        
#Agent_state
cdef extern from "./agent/agent.hpp":
    cdef struct Agent_collision_box:
        float b_height
        float c_height
        float box_r

    cdef cppclass Agent_state:
        int id
        AgentState s
        Agent_collision_box box
        Agent_status status
        void teleport(float x,float y,float z)
        void send_id_to_client(int client_id)

cdef extern from "./agent/agent.hpp":
    cdef cppclass Agent_list:
        Agent_state* get(int id)
        Agent_state* get_or_create(int id)
        Agent_state* create()
        Agent_state* create(int id)
        void destroy(int _id)
        void draw()
        void draw(int all)

cdef extern from "./state/server_state.hpp" namespace "ServerState":
    Agent_list agent_list

class AgentWrapper(object):
    properties = [
        'x', 'y', 'z',
        'vx', 'vy', 'vz',
        'theta', 'phi',
        'x_angle', 'y_angle',
        'crouch_height','c_height',
        'box_height', 'b_height',
        'box_r',
        #'crouching',
        'team',
        'score',
        'kills',
        'deaths',
        'suicides',
        'name',
    ]

    def __init__(self, int client_id):
        cdef Agent_state *a
        a = agent_list.get_or_create(client_id)
        self.id = a.id

    def send_id_to_client(self, int client_id):
        cdef Agent_state* a
        a = agent_list.get(self.id)
        a.send_id_to_client(client_id)
        
    def __getattribute__(self, name):
        if name not in AgentWrapper.properties:
            raise AttributeError

        cdef Agent_state* a
        cdef int i
        i = object.__getattribute__(self,'id')
        a = agent_list.get(i)

        if a == NULL:
            print "AgentWrapper.__getattribute__ :: agent %d not found" % (i,)
            raise AttributeError

        if name == 'x':
            return a.s.x
        elif name == 'y':
            return a.s.y
        elif name == 'z':
            return a.s.z
        elif name == 'vx':
            return a.s.vx
        elif name == 'vy':
            return a.s.vy
        elif name == 'vz':
            return a.s.vz
        elif name == 'theta':
            return a.s.theta
        elif name == 'phi':
            return a.s.phi

        elif name == 'x_angle': # legacy reasons
            return a.s.theta
        elif name == 'y_angle':
            return a.s.phi
            
        elif name == 'crouch_height' or name == 'c_height':
            return a.box.c_height
        elif name == 'box_height' or name == 'b_height':
            return a.box.b_height
        elif name == 'box_r':
            return a.box.box_r

        elif name == 'team':
            return a.status.team

        elif name == 'score':
            return a.status.score()
        elif name == 'kills':
            return a.status.kills
        elif name == 'deaths':
            return a.status.deaths
        elif name == 'suicides':
            return a.status.suicides

        elif name == 'name':
            return a.status.name

        #elif name == 'crouching':
        #    return a.s.crouching

        raise AttributeError

#def set_agent_name(int id, name):
#    name = name[:PLAYER_NAME_MAX_LENGTH]
#    cdef Agent_state* a
#    a = agent_list.get(id)
#    if a == NULL:
#        print "cAgents -- set_agent_name, agent %d unknown. Name=%s" % (id, name,)
#        return
#    a.status.set_name(name)

def client_identified(int client_id):
    cdef Agent_state* a
    a = agent_list.get(client_id)
    if a == NULL:
        return False
    return a.status.identified
    

#functions

def teleport_Agent(int id, float x, float y, float z):
    cdef Agent_state* a
    a = agent_list.get(id)
    if a != NULL:
        a.teleport(x,y,z)
    else:
        print "Cannot teleport agent: agent %i does not exist" %(id)

class AgentListWrapper:

    @classmethod
    def add(cls, int id):
        cdef Agent_state* a
        a = agent_list.get_or_create(id)
        return a.id

    @classmethod
    def remove(cls, int id):
        agent_list.destroy(id)
        return id


""" Game Modes (CTF) """

from libcpp cimport bool
cdef extern from "./game/teams.hpp":
    cdef cppclass Team:
        int id
        char* name
        int n
        bool viewers

    cdef cppclass NoTeam:
        int id
        char* name
        int n
        bool viewers

    cdef cppclass CTFTeam:  #inherits Team
        pass

cdef extern from "./game/ctf.hpp":
    cdef cppclass CTF:
        NoTeam none
        CTFTeam one
        CTFTeam two
        void set_team_color(int team, unsigned char r, unsigned char g, unsigned char b)
        void add_agent_to_team(int team, int agent)
        void remove_agent_from_team(int agent)
        void start()
        void check_agent_proximities()
        void set_team_name(int team, char* name)

cdef extern from "./state/server_state.hpp" namespace "ServerState":
    CTF ctf

def set_team_color(int team, unsigned char r, unsigned char g, unsigned char b):
    ctf.set_team_color(team, r,g,b)

class DummyCTFTeam(object):

    def __init__(self, id):
        if id not in [1,2]:
            print "DummyCTFTeam object -- id invalid %d" % (id,)
            raise ValueError
        self.id = id

    def __getattribute__(self, k):

        id = object.__getattribute__(self, 'id')
        cdef Team t
        if id == 1:
            t = <Team>(ctf.one)
        elif id == 2:
            t = <Team>(ctf.two)

        elif k == 'name':
            return t.name
        elif k == 'n':
            return t.n
        elif k == 'viewers':
            return t.viewers
        elif k == 'id':
            return k.id
        elif k.startswith('__'):
            return object.__getattribute__(self, k)

        raise AttributeError

ctf_one = DummyCTFTeam(1)
ctf_two = DummyCTFTeam(2)

class DummyNoTeam(object):

    def __getattribute__(self, k):

        cdef NoTeam t
        t = ctf.none

        if k == 'name':
            return t.name
        elif k == 'n':
            return t.n
        elif k == 'viewers':
            return t.viewers
        elif k == 'id':
            return k.id
        elif k.startswith('__'):
            return object.__getattribute__(self, k)
        raise AttributeError

ctf_none = DummyNoTeam()

def get_team(int id):
    if id == 0:
        return ctf_none
    elif id == 1:
        return ctf_one
    elif id == 2:
        return ctf_two
    else:
        print "c_lib_game_modes.get_team :: invalid team id %d" % (id,)


def join_team(int agent_id, int team_id):
    ctf.add_agent_to_team(team_id, agent_id)

def leave_team(int agent_id):
    ctf.remove_agent_from_team(agent_id)

def ctf_start():
    ctf.start()
    
def check_agent_proximities():
    ctf.check_agent_proximities()

""" Particles """

cdef extern from "./physics/vector.hpp":
    struct Vector:
        float x
        float y
        float z

cdef extern from "./physics/common.hpp":
    struct State:
        Vector p
        Vector v

cdef extern from "./particles/particles.hpp":
    cdef struct Particle2:
        State state
        unsigned int id

cdef extern from "./particles/grenade.hpp":
    cdef cppclass Grenade:
        Particle2 particle

    cdef cppclass Grenade_list:
        Grenade* get(int id)
        Grenade* create()
        Grenade* create(int id)
        Grenade* create(float x, float y, float z, float vx, float vy, float vz)
        Grenade* create(int id, float x, float y, float z, float vx, float vy, float vz)
        void destroy(int id)
        void tick()

cdef extern from "./particles/cspray.hpp":
    cdef cppclass Cspray:
        Particle2 particle

    cdef cppclass Cspray_list:
        Cspray* get(int id)
        Cspray* create()
        Cspray* create(int id)
        Cspray* create(float x, float y, float z, float vx, float vy, float vz)
        Cspray* create(int id, float x, float y, float z, float vx, float vy, float vz)
        void destroy(int id)
        void tick()

cdef extern from "./particles/neutron.hpp":
    cdef cppclass Neutron:
        Particle2 particle
        void set_energy(int energy)

    cdef cppclass Neutron_list:
        Neutron* get(int id)
        Neutron* create()
        Neutron* create(int id)
        Neutron* create(float x, float y, float z, float vx, float vy, float vz)
        Neutron* create(int id, float x, float y, float z, float vx, float vy, float vz)
        void destroy(int id)
        void draw()
        void tick()


cdef extern from "./state/server_state.hpp" namespace "ServerState":
    Grenade_list grenade_list
    Cspray_list cspray_list
    Neutron_list neutron_list

def tick():
    grenade_list.tick()
    cspray_list.tick()
    neutron_list.tick()
    
def _create_neutron(int type, int energy, float x, float y, float z, float vx, float vy, float vz):
    cdef Neutron* neutron
    neutron = neutron_list.create(x,y,z, vx,vy,vz)
    if neutron == NULL: return
    neutron.set_energy(energy)

def _create_cspray(float x, float y, float z, float vx, float vy, float vz):
    cspray_list.create(x,y,z, vx,vy,vz)

""" Options & Settings """

from libcpp cimport bool

cdef extern from "./game/game.hpp":
    void set_team_kills(bool team_kills)

def load(opts):
    set_team_kills(opts.team_kills)

    ctf.set_team_name(1, opts.team_name_one)
    ctf.set_team_name(2, opts.team_name_two)
