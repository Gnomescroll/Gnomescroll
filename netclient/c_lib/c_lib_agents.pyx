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




'''
DONT DEPRECATE BELOW
'''

cdef extern from "./agent/agent.hpp":
    void agents_draw()
    int agent_create(int id, float x, float y, float z)
    void init_agent_vox_part(int id, int part, unsigned short vox_x, unsigned short vox_y, unsigned short vox_z, float vox_size)
    void set_agent_vox_volume(int id, int part, int x, int y, int z, int r, int g, int b, int a)
    void set_agent_limb_direction(int id, int part, float fx, float fy, float fz, float nx, float ny, float nz)
    void set_agent_limb_anchor_point(int id, int part, float length, float ax, float ay, float az)
    void init_agent_vox_done(int id)

import dat.agent_dim as dat
# import dat.lu1, dat.lu2, dat.lu3, vosize, skel_tick
vosize = dat.vosize

def create_ag(int id, float x, float y, float z):
    agent_create(id, x,y,z)
    _init_agent_vox(id)

def update_ag(int id):
    _update_agent_vox(id)

PART_NUM = 6
def _init_agent_vox(int id):
    #global dat.lu1, dat.lu2, dat.lu3,
    global vosize
    global PART_NUM

    for part in range(PART_NUM):
        xdim,ydim,zdim = dat.lu1[part]

        init_agent_vox_part(id, part, xdim, ydim, zdim, vosize)

        for x in range(xdim):
            for y in range(ydim):
                for z in range(zdim):
                    a = 255
                    r = 32*x
                    g = 32*y
                    b = 32*z
                    set_agent_vox_volume(id, part, x,y,z, r,g,b,a) # THIS

    for part in range(PART_NUM):
        length, ax,ay,az= dat.lu2[part]
        set_agent_limb_anchor_point(id, part, length,ax,ay,az)
    for part in range(PART_NUM):
        fx,fy,fz, nx,ny,nz = dat.lu3[part]
        set_agent_limb_direction(id, part, fx, fy, fz, nx,ny,nz)

    init_agent_vox_done(id)

def _update_agent_vox(int id):
    dat.skel_tick()
    for part in range(PART_NUM):
        length, ax,ay,az= dat.lu2[part]
        set_agent_limb_anchor_point(id, part, length,ax,ay,az)
    for part in range(PART_NUM):
        fx,fy,fz,nx,ny,nz = dat.lu3[part]
        set_agent_limb_direction(id, part, fx, fy, fz, nx,ny,nz)


def draw_agents():
    agents_draw()
