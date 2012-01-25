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
        void set_name(char* name, int id)
        
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
        Agent_state* create()
        Agent_state* create(int id)
        void destroy(int _id)
        void draw()
        void draw(int all)
        void send_to_client(int client_id)

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
        a = agent_list.create(client_id)
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

def set_agent_name(int id, name):
    name = name[:PLAYER_NAME_MAX_LENGTH]
    cdef Agent_state* a
    a = agent_list.get(id)
    if a == NULL:
        print "cAgents -- set_agent_name, agent %d unknown. Name=%s" % (id, name,)
        return
    a.status.set_name(name, id)

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
    def add(cls):
        cdef Agent_state* a
        a = agent_list.create()
        return a.id

    @classmethod
    def remove(cls, int id):
        agent_list.destroy(id)
        return id

    @classmethod
    def send_to_client(cls, int client_id):
        agent_list.send_to_client(client_id)
        
