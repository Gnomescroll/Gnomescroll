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
    Agent_state* create_agent()
    Agent_state* get_agent(int id)
    void delete_agent(int id)

### make agent wrapper
#getters and settings

### 



#functions

#create agent and return id
def _create_agent():
    cdef Agent_state* a
    a = create_agent()
    return a.id

cdef Agent_state* _get_agent(int id):
    cdef Agent_state* a
    a = get_agent(id)
    return a.id

def _delete_agent(int id):
    delete_agent(id)