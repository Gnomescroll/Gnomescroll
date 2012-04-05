from libcpp cimport bool

cdef extern from "c_lib.hpp":
    int init_c_lib()
    void close_c_lib()
    void _set_seed(int seed)
    
def close():
    close_c_lib()

def init():
    init_c_lib()

def reset_seed(int seed):
    _set_seed(seed)

## net stuff
cdef extern from "./net_lib/host.hpp":
#    void init_net_server(int a, int b, int c, int d, int port)
    void server_dispatch_network_events()
    void flush_to_net()

#def NetServerInit(int a=0, int b=0, int c=0, int d=0, int port=0):
#    init_net_server(a,b,c,d,port)

def NetServerDispatchNetworkEvents():
    server_dispatch_network_events()

def NetServerFlushToNet():
    flush_to_net()

"""
Options
"""

#cdef extern from "./options.hpp" namespace "Options":
#    void set_server_name(char* server_name)
#    void set_ip_address(char* ip_address)
#    void set_port(int port)
#    void set_game_mode(char* game_mode)
#    void set_team_kills(bool team_kills)
#    void set_victory_points(int victory_points)
#    void set_team_name_one(char* team_name_one)
#    void set_team_name_two(char* team_name_two)
#    void set_map(char* map)
#    void set_seed(unsigned int seed)
#    void set_logger(bool logger)

#def load_options(opts):
#    set_server_name(opts.server_name)
#    set_ip_address(opts.ip_address)
#    set_port(opts.port)
#    set_game_mode(opts.game_mode)
#    set_team_kills(opts.team_kills)
#    set_victory_points(opts.victory_points)
#    set_team_name_one(opts.team_name_one)
#    set_team_name_two(opts.team_name_two)
#    set_map(opts.map)
#    set_seed(opts.seed)
#    reset_seed(opts.seed)
#    set_logger(opts.logger)

#new functions
cdef extern from "../c_lib/time/physics_timer.h":
    void _START_CLOCK()
    int _GET_TICK()
    int _GET_MS_TIME()


def START_CLOCK():
    _START_CLOCK()

def GET_TICK():
    return _GET_TICK()

def GET_MS_TIME():
    return _GET_MS_TIME();


"""
    Condensed cython files here
"""

""" Monsters """
cdef extern from "./monsters/monsters.hpp" namespace "Monsters":
    cdef cppclass Slime_list:
        void tick()
    void test(int n)
    void populate_slimes(int n_max)

cdef extern from "./state/server_state.hpp" namespace "ServerState":
    Slime_list* slime_list

def slime_test(int n):
    test(n)

def slime_populate(int n_max):
    populate_slimes(n_max)

def slime_tick():
    if slime_list != NULL:
        slime_list.tick()

""" Agents """
cdef extern from "./state/server_state.hpp" namespace "ServerState":
    char* agent_name(int id)

def get_agent_name(int id):
    name = agent_name(id)
    if name == NULL:
        return ''
    return name

""" Game Modes (CTF) """

cdef extern from "./game/ctf.hpp":
    cdef cppclass CTF:
        void set_team_color(int team, unsigned char r, unsigned char g, unsigned char b)
        void start()
        void check_agent_proximities()
        void update()

cdef extern from "./state/server_state.hpp" namespace "ServerState":
    CTF* ctf

def set_team_color(int team, unsigned char r, unsigned char g, unsigned char b):
    if ctf != NULL:
        ctf.set_team_color(team, r,g,b)
    
def check_agent_proximities():
    if ctf != NULL:
        ctf.check_agent_proximities()
        ctf.update()

"""
Terrain map
"""

cdef extern from "../c_lib/t_map/t_map.hpp":
    int _get(int x, int y, int z)
    void _set(int x, int y, int z, int value)

'''
Part 1: State

'''

cpdef inline set(int x,int y, int z,int value):
    _set(x,y,z,value)

cpdef inline int get(int x, int y,int z):
    return _get(x,y,z)

'''
    Init Stuff
'''

cdef extern from "./state/server_state.hpp" namespace "ServerState":
    void server_tick()
    void start_game()

def tick_server_state():
    server_tick();
def start():
    start_game()
