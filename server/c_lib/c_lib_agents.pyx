cdef extern from "./agent/agent.h":
    void agent_tick()
    #int create_agent(float x, float y, float z)
    #void set_agent_state(int id, float xangle, float yangle)

cdef extern from "./agent/agent_vox.h":
    void init_agent_vox_volume(int id, int part, int xdim, int ydim, int zdim, float vosize)
    void set_agent_vox_volume(int id, int part, int x, int y, int z, int r, int g, int b, int a)

    void set_agent_limb_direction(int id, int part, float fx,float fy,float fz, float nx,float ny, float nz)
    void set_agent_limb_anchor_point(int id, int part, float length, float ax,float ay,float az)

'''
cdef extern from "./agent/agent_client.h":
    void set_agent_control_state(int x[32], float theta, float phi)
'''

'''
def _create_agent(float x, float y, float z):
    cdef int id
    id = create_agent(x,y,z)
'''

'''
def _set_agent_state(int id, float xangle, float yangle):
    set_agent_state(id, xangle, yangle)
'''

'''
def _set_agent_control_state(int f, int b, int l, int r, int jump, float theta, float phi):
    cdef int x[32]
    x[0] = f
    x[1] = b
    x[2] = l
    x[3] = r
    x[4] = jump
    set_agent_control_state(x, theta, phi)
'''
