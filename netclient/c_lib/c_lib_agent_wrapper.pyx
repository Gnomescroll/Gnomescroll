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

#agent list wrapper
cdef extern from "./state/wrapper.hpp":
    Agent_state* create_agent(int id) #for client
    Agent_state* get_or_create_agent(int id)
    Agent_state* get_agent(int id)
    void delete_agent(int id)

from c_lib.c_lib_agents import _init_agent_vox

agent_props = ['theta', 'phi', 'x', 'y', 'z', 'vx', 'vy', 'vz', 'x_angle', 'y_angle']

class AgentWrapper(object):

    def __init__(self, id):
        AgentListWrapper._create(id)
        self.id = id
        _init_agent_vox(id)
        
    def __getattribute__(self, name):
        if name == 'id':
            return object.__getattribute__(self, name)
            
        if name not in agent_props:
            raise AttributeError

        cdef Agent_state* a
        a = get_agent(object.__getattribute__(self, 'id'))
        
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

#create agent and return id
def _create_agent(int id):
    cdef Agent_state* a
    a = get_or_create_agent(id)
    return a.id

cdef Agent_state* _get_agent(int id):
    cdef Agent_state* a
    a = get_agent(id)
    return a

def _delete_agent(int id):
    delete_agent(id)
    return id

class AgentListWrapper:

    @classmethod
    def _create(cls, int id):
        return _create_agent(id)

    @classmethod
    def _remove(cls, int id):
        return _delete_agent(id)

#control state
cdef extern from "./state/client_state.hpp" namespace "ClientState":
    void set_control_state(int f, int b, int l, int r, int jet, int jump, float theta, float phi)
    void set_PlayerAgent_id(int id)

def set_agent_control_state(int f, int b, int l, int r, int jet, int jump, float theta, float phi):
    set_control_state(f,b,l,r,jet,jump,theta,phi)

def set_player_agent_id(int id):
    print "SETTING player agent id %d" % (id,)
    set_PlayerAgent_id(id)
