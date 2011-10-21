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
    Agent_state* create_agent()         #for server
    Agent_state* get_agent(int id)
    void delete_agent(int id)

### make agent wrapper
#getters and settings

###

agent_props = ['id', 'theta', 'phi', 'x', 'y', 'z', 'vx', 'vy', 'vz']

class AgentWrapper(object):

    def __init__(self, id):
        AgentListWrapper._create(id)
        self.id = id
        
    def __getattribute__(self, name):
        if name not in agent_props:
            raise AttributeError

        cdef Agent_state* a
        a = get_agent(self.id)
        if name == 'id':
            self.id = id
            return a.id
        else:
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

#functions

#create agent and return id
def _create_agent():
    cdef Agent_state* a
    a = create_agent()
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
    def _add(cls):
        return _create_agent()

    @classmethod
    def _remove(cls, int id):
        return _delete_agent(id)
        
