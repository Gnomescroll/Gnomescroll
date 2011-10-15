cdef extern from "./state/client_state.hpp" namespace "ClientState":
    void set_control_state(int f, int b, int l, int r, int jet, int jump, float theta, float phi)

def set_agent_control_state(int f, int b, int l, int r, int jet, int jump, float theta, float phi):
    set_control_state(f,b,l,r,jet,jump,theta,phi)

'''
DEPRECATE
'''


cdef extern from "./agent/agent_draw.hpp" namespace "AgentDraw":
    void draw_agent_aiming_direction(float x, float y, float z, float xangle, float yangle)
    void draw_agent_bounding_box(float x, float y, float z, float radius, float head_height, float height)
    void draw_agent_cube_selection(int x, int y, int z, int r, int g, int b)
    void draw_agent_cube_side_selection(int x, int y, int z, int cx, int cy, int cz, int r, int g, int b)

def _draw_agent_aiming_direction(float x, float y, float z, float xangle, float yangle):
    draw_agent_aiming_direction(x, y, z, xangle, yangle)

def _draw_agent_bounding_box(float x, float y, float z, float radius, float head_height, float height):
    draw_agent_bounding_box(x, y, z, radius, head_height, height)

def _draw_agent_cube_selection(int x, int y, int z, int r, int g, int b):
    draw_agent_cube_selection(x,y,z, r,g,b)

def _draw_agent_cube_side_selection(int x, int y, int z, int cx, int cy, int cz, int r, int g, int b):
    draw_agent_cube_side_selection( x,  y,  z,  cx,  cy,  cz,  r,  g,  b)



#agent

'''
from dat.agent_dim import lu1, lu2, lu3, vosize, skel_tick
agent_list = []

def agent_skeleton_update():
    global agent_list,lu2,lu3
    skel_tick()
    for id in agent_list:
        for part in range(0,6):
            length, ax,ay,az= lu2[part]
            set_agent_limb_anchor_point(id, part, length,ax,ay,az)
        for part in range(0,6):
            fx,fy,fz,nx,ny,nz = lu3[part]
            set_agent_limb_direction(id, part, fx, fy, fz, nx,ny,nz)
'''



'''
def default_vox_model_init(int id, int part, int xdim, int ydim, int zdim, float vosize):
    init_agent_vox_volume(id, part, xdim,ydim,zdim, vosize)
    for x in range(0,xdim):
        for y in range(0,ydim):
            for z in range(0,zdim):
                a = 255
                r = 32*x
                g = 32*y
                b = 32*z
                set_agent_vox_volume(id, part, x,y,z, r,g,b,a)
'''

'''
def _set_agent_model(int id):
    #cdef float vosize = .0625
    cdef int part
    cdef int xdim, ydim, zdim

    global lu1, lu2, lu3, vosize

    for part in range(0,6):
        xdim,ydim,zdim = lu1[part]
        default_vox_model_init(id, part, xdim,ydim,zdim, vosize)

    for part in range(0,6):
        length, ax,ay,az= lu2[part]
        set_agent_limb_anchor_point(id, part, length,ax,ay,az)
    for part in range(0,6):
        fx,fy,fz, nx,ny,nz = lu3[part]
        set_agent_limb_direction(id, part, fx, fy, fz, nx,ny,nz)

def _create_agent(float x, float y, float z):
    cdef int id
    id = create_agent(x,y,z)
    _set_agent_model(id)
    ##
    global agent_list
    agent_list.append(id)
    return id
'''

cdef extern from "./agent/agent.hpp":
    void agents_tick()
    void agents_draw()
    int agent_create(int id, float x, float y, float z)

def _create_agent(int id, float x, float y, float z):
    print "Created agent %d" % agent_create(id, x,y,z)

def draw_agents():
    agents_draw()
