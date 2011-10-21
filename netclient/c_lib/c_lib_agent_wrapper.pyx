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
    Agent_state* get_agent(int id)
    void delete_agent(int id)

### make agent wrapper
#getters and settings

### 



#functions

#create agent and return id
def _create_agent(int id):
    cdef Agent_state* a
    a = create_agent(id)
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
    def _create(int id):
        return _create_agent(id)

    @classmethod
    def _remove(int id):
        return _delete_agent(id)
