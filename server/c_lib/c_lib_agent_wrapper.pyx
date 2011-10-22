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


### make agent wrapper
#getters and settings

### 



#functions

#create agent and return id
def _create_agent():
    cdef Agent_state* a
    a = create_agent()
    return a.id
