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
cdef extern from "./physics/vector.h":
    cdef struct Vector:
        float x
        float y
        float z

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
        float camera_height
        AgentState s
        void teleport(float x,float y,float z)
        Vector interpolate
        void set_interpolated(int t)


cdef extern from "./agent/agent.hpp":
    void init_agent_vox_part(int id, int part, unsigned short vox_x, unsigned short vox_y, unsigned short vox_z, float vox_size)
    void set_agent_vox_volume(int id, int part, int x, int y, int z, int r, int g, int b, int a)
    void set_agent_limb_direction(int id, int part, float fx, float fy, float fz, float nx, float ny, float nz)
    void set_agent_limb_anchor_point(int id, int part, float length, float ax, float ay, float az)
    void init_agent_vox_done(int id)

    void init_agents_to_draw()
    void clear_agents_to_draw()
    void set_agents_to_draw(int* ids, int ct)

    void set_agent_tick_mode(int mode)
    cdef enum tick_modes:
        use_jetpack
        use_jump

cdef extern from "./agent/agent.hpp":
    cdef cppclass Agent_list:
        void draw()
        void draw(int all)
        Agent_state* get(int id)
        Agent_state* create()
        Agent_state* create(int id)
        Agent_state* get_or_create(int id)
        void destroy(int _id)
        void where()

cdef extern from "./state/client_state.hpp" namespace "ClientState":
    Agent_list agent_list
    void set_control_state(int f, int b, int l, int r, int jet, int jump, int crouch, int boost, int misc1, int misc2, int misc3, float theta, float phi)
    void set_PlayerAgent_id(int id)

def init_draw_agents():
    init_agents_to_draw()

def draw_agents():
    agent_list.draw()
def jump_physics():
    set_agent_tick_mode(use_jump)
def jetpack_physics():
    set_agent_tick_mode(use_jetpack)

import dat.agent_dim as dat
# import dat.lu1, dat.lu2, dat.lu3, vosize, skel_tick
vosize = dat.vosize

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
    return # !!!
    dat.skel_tick()
    for part in range(PART_NUM):
        length, ax,ay,az= dat.lu2[part]
        set_agent_limb_anchor_point(id, part, length,ax,ay,az)
    for part in range(PART_NUM):
        fx,fy,fz,nx,ny,nz = dat.lu3[part]
        set_agent_limb_direction(id, part, fx, fy, fz, nx,ny,nz)

def load_agents_to_draw(agents):
    clear_agents_to_draw()
    cdef int a[1024]
    cdef int i = 0
    for ag in agents:
        a[i] = ag
        i += 1
    set_agents_to_draw(a, i)


'''
WRAPPER
'''

agent_props = ['theta', 'phi', 'x', 'y', 'z', 'vx', 'vy', 'vz', 'x_angle', 'y_angle', 'camera_height']

class AgentWrapper(object):

    def __init__(self, int id):
        agent_list.create(id)
        self.id = id
        
    def __getattribute__(self, name):
        if name not in agent_props:
            raise AttributeError

        cdef Agent_state* a
        a = agent_list.get(object.__getattribute__(self,'id'))

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

        elif name == 'camera_height':
            return a.camera_height

    def update_interpolate(self, int t):
        cdef Agent_state* a
        a = agent_list.get(object.__getattribute__(self,'id'))
        a.set_interpolated(t)

    def interpolated(self):
        cdef Agent_state* a
        cdef float x
        cdef float y
        cdef float z

        a = agent_list.get(object.__getattribute__(self,'id'))

        x = a.interpolate.x
        y = a.interpolate.y
        z = a.interpolate.z

        return [x,y,z]

#functions

def teleport_Agent(int id, float x, float y, float z):
    cdef Agent_state* a
    a = agent_list.get(id)
    if a != NULL:
        a.teleport(x,y,z)
    else:
        print "Cannot teleport agent: agent %i does not exist" % (id,)

def set_agent_control_state(int f, int b, int l, int r, int jet, int jump, int crouch, int boost, int misc1, int misc2, int misc3, float theta, float phi):
    set_control_state(f,b,l,r,jet,jump,crouch, boost, misc1, misc2, misc3, theta,phi)

def set_player_agent_id(int id):
    set_PlayerAgent_id(id)


class AgentListWrapper:

    @classmethod
    def add(cls, int id):
        agent_list.get_or_create(id)
        return id

    @classmethod
    def remove(cls, int id):
        agent_list.destroy(id)
        return id

