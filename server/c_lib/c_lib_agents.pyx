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
        bool crouching
        bool jump_ready
        
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
        void teleport(float x,float y,float z)

cdef extern from "./agent/agent.hpp":
    cdef cppclass Agent_list:
        Agent_state* get(int id)
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
        'crouching'
    ]

#    cdef int id
    def __init__(self):
        cdef Agent_state *a
        a = agent_list.create()
        self.id = a.id
        
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

        elif name == 'crouching':
            return a.s.crouching

        raise AttributeError


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
        
