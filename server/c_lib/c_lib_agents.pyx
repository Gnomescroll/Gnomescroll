'''
Wrapper
'''

#agent class wrapper

#AgentState
cdef extern from "./agent/agent.hpp":
    cdef cppclass AgentState:
        int seq
        float theta
        float phi
        float x,y,z
        float vx,vy,vz

#Agent_state
cdef extern from "./agent/agent.hpp":
    cdef cppclass Agent_state:
        int id
        AgentState s
        void teleport(float x,float y,float z)


cdef extern from "./agent/agent.hpp":
    cdef cppclass Agent_list:
        Agent_state* get(int id)
        Agent_state* create()
        Agent_state* create(int id)
        void destroy(int _id)
        void draw()
        void draw(int all)

#cdef extern from "./state/server_state.hpp" namespace "ServerState":
cdef extern from "./state/cython_imports.hpp" namespace "ServerState":
    Agent_list agent_list


agent_props = ['theta', 'phi', 'x', 'y', 'z', 'vx', 'vy', 'vz', 'x_angle', 'y_angle']

class AgentWrapper(object):

    def __init__(self):
        cdef Agent_state *a
        a = agent_list.create()
        self.id = a.id
        
    def __getattribute__(self, name):
        if name not in agent_props:
            raise AttributeError

        cdef Agent_state* a
        #a = C_get_agent(object.__getattribute__(self,'id'))
        a = agent_list.get(object.__getattribute__(self,'id'))
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
        
