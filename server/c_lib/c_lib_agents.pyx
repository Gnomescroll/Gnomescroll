#cdef extern from "./agent/agent.h":
cdef extern:
    void agent_tick()
    #int create_agent(float x, float y, float z)
    #void set_agent_state(int id, float xangle, float yangle)

#cdef extern from "./agent/agent_vox.h":
cdef extern:
    void init_agent_vox_volume(int id, int part, int xdim, int ydim, int zdim, float vosize)
    void set_agent_vox_volume(int id, int part, int x, int y, int z, int r, int g, int b, int a)

    void set_agent_limb_direction(int id, int part, float fx,float fy,float fz, float nx,float ny, float nz)
    void set_agent_limb_anchor_point(int id, int part, float length, float ax,float ay,float az)


'''
def _create_agent(float x, float y, float z):
    cdef int id
    id = create_agent(x,y,z)
'''