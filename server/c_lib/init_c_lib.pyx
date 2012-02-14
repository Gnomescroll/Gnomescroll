from libcpp cimport bool

cdef extern from "c_lib.hpp":
    int init_c_lib()
    void close_c_lib()

def close():
    close_c_lib()

def init():
    init_c_lib()

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
    long _get_time()
    long _get_tick()

#new functions
cdef extern from "../c_lib/time/physics_timer.h":
    void _START_CLOCK()
    int _GET_TICK()
    int _GET_MS_TIME()

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

cdef extern from "c_lib.hpp":
    void _set_seed(int seed)
    
def set_seed(int seed):
    _set_seed(seed)

cdef extern from "./state/server_state.hpp" namespace "ServerState":
    void server_tick()

def tick_server_state():
    server_tick();

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
cdef extern from "./agent/agent_status.hpp":
    cdef cppclass Agent_status:
        char* name
        bool identified
        
cdef extern from "./agent/agent.hpp":
    cdef cppclass Agent_state:
        int id
        Agent_status status
        void teleport(float x,float y,float z)

cdef extern from "./agent/agent.hpp":
    cdef cppclass Agent_list:
        Agent_state* get(int id)
        Agent_state* create(int id)
        void destroy(int id)

cdef extern from "./state/server_state.hpp" namespace "ServerState":
    Agent_list agent_list

def get_agent_name(int id):
    cdef Agent_state* a
    a = agent_list.get(id)
    return a.status.name

def destroy_agent(int id):
    agent_list.destroy(id)

def teleport_Agent(int id, float x, float y, float z):
    cdef Agent_state* a
    a = agent_list.get(id)
    if a != NULL:
        a.teleport(x,y,z)
    else:
        print "Cannot teleport agent: agent %i does not exist" %(id)

""" Game Modes (CTF) """

cdef extern from "./game/ctf.hpp":
    cdef cppclass CTF:
        void set_team_color(int team, unsigned char r, unsigned char g, unsigned char b)
        void start()
        void check_agent_proximities()
        void set_team_name(int team, char* name)

cdef extern from "./state/server_state.hpp" namespace "ServerState":
    CTF ctf

def set_team_color(int team, unsigned char r, unsigned char g, unsigned char b):
    ctf.set_team_color(team, r,g,b)

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

cdef extern from "./state/server_state.hpp" namespace "ServerState":
    Grenade_list grenade_list
    Cspray_list cspray_list

def tick():
    grenade_list.tick()
    cspray_list.tick()
 
def _create_cspray(float x, float y, float z, float vx, float vy, float vz):
    cspray_list.create(x,y,z, vx,vy,vz)

""" Options & Settings """

cdef extern from "./game/game.hpp":
    void set_team_kills(bool team_kills)

def load(opts):
    set_team_kills(opts.team_kills)

    ctf.set_team_name(1, opts.team_name_one)
    ctf.set_team_name(2, opts.team_name_two)


"""
Terrain map
"""
cdef extern from "../c_lib/t_map/t_map.hpp" namespace "t_map":
    void init_t_map()

cdef extern from "../c_lib/t_map/t_map.hpp":
    int _get(int x, int y, int z)
    void _set(int x, int y, int z, int value)

#called automaticly after properties are loaded
def _init_map():
    init_t_map()

'''
PART 2: Properties
'''

cdef extern from "./t_map/t_properties.hpp" namespace "t_map":
    struct cubeProperties:
        bint active
        bint solid
        bint occludes
        bint transparent
        bint reserved5
        bint reserved6
        bint reserved7
        bint reserved8
        unsigned char max_damage

cdef extern from "./t_map/t_properties.hpp" namespace "t_map":
    cubeProperties* get_cube(int id)

'''
Part 1: State

'''

cpdef inline set(int x,int y, int z,int value):
    _set(x,y,z,value)

cpdef inline int get(int x, int y,int z):
    return _get(x,y,z)


import dats.loader as dat_loader
c_dat = dat_loader.c_dat

def init_cube_properties(c_dat):

    def apply(id):
        global infinite_texture_counter
        cdef cubeProperties* cp
        cp = get_cube(id)
        cp.active = int(c_dat.get(id,'active'))
        cp.solid = int(c_dat.get(id,'solid'))
        cp.occludes = int(c_dat.get(id,'occludes'))
        cp.transparent = int(c_dat.get(id,'transparent'))
        cp.max_damage = int(c_dat.get(id,'max_damage'))

    for id in c_dat.dat:
        apply(id)


'''
    Init Stuff
'''

def init_terrain():
    global c_dat
    print "Init Terrain Map"
    init_cube_properties(c_dat)
    init_t_map()
init_terrain()

