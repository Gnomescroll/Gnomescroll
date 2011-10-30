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
        void where()

cdef extern from "./state/client_state.hpp" namespace "ClientState":
    Agent_list agent_list

agent_props = ['theta', 'phi', 'x', 'y', 'z', 'vx', 'vy', 'vz', 'x_angle', 'y_angle']

class AgentWrapper(object):

    def __init__(self, int id):
        agent_list.create(id)
        self.id = id
        
    def __getattribute__(self, name):
        if name not in agent_props:
            raise AttributeError

        cdef Agent_state* a
        a = agent_list.get(object.__getattribute__(self,'id'))
        agent_list.where()
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

#control state
cdef extern from "./state/client_state.hpp" namespace "ClientState":
    void set_control_state(int f, int b, int l, int r, int jet, int jump, float theta, float phi)
    void set_PlayerAgent_id(int id)

def set_agent_control_state(int f, int b, int l, int r, int jet, int jump, float theta, float phi):
    set_control_state(f,b,l,r,jet,jump,theta,phi)

def set_player_agent_id(int id):
    set_PlayerAgent_id(id)


class AgentListWrapper:

    @classmethod
    def add(cls, int id):
        agent_list.create(id)
        return id

    @classmethod
    def remove(cls, int id):
        agent_list.destroy(id)
        return id
