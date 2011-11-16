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

from libcpp cimport bool
#AgentState
cdef extern from "./agent/agent.hpp":
    cdef cppclass AgentState:
        int seq
        float theta
        float phi
        float x,y,z
        float vx,vy,vz
        float camera_height
        bool crouching
        bool jump_ready

#Agent_state
cdef extern from "./agent/agent.hpp":
    cdef struct Agent_collision_box:
        float b_height
        float c_height
        float box_r

    cdef cppclass Agent_state:
        int id
        AgentState s
        Agent_collision_box box
        #void teleport(float x,float y,float z)

cdef extern from "./agent/player_agent.hpp":
    cdef enum active_camera_states:
        smoothed
        client_side_prediction_interpolated
        client_side_prediction
        last_snapshot

    cdef cppclass PlayerAgent_state:
        int agent_id
        float camera_height()
        void calculate_smoothing()
        AgentState* active_camera_state
        void set_active_camera_state(int type)
        
cdef extern from "./agent/agent.hpp":
    void init_agent_vox_part(int id, int part, unsigned short vox_x, unsigned short vox_y, unsigned short vox_z, float vox_size)
    void set_agent_vox_volume(int id, int part, int x, int y, int z, int r, int g, int b, int a)
    void set_agent_limb_direction(int id, int part, float fx, float fy, float fz, float nx, float ny, float nz)
    void set_agent_limb_anchor_point(int id, int part, float length, float ax, float ay, float az)
    void init_agent_vox_done(int id)

    void init_agents_to_draw()
    void clear_agents_to_draw()
    void set_agents_to_draw(int* ids, int ct)

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
    PlayerAgent_state playerAgent_state

def init_draw_agents():
    init_agents_to_draw()

def draw_agents():
    agent_list.draw()

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


class AgentWrapper(object):
    properties = [
        'x', 'y', 'z',
        'vx', 'vy', 'vz',
        'theta', 'phi',
        'x_angle', 'y_angle',
        'crouch_height','c_height',
        'box_height', 'b_height',
        'box_r',
        'crouching'
    ]

    def __init__(self, int id):
        agent_list.create(id)
        self.id = id
        
    def __getattribute__(self, name):
        if name not in AgentWrapper.properties:
            raise AttributeError

        cdef Agent_state* a
        cdef int i
        i = object.__getattribute__(self,'id')
        a = agent_list.get(i)

        if a == NULL:
            print "AgentWrapper.__getattribute__ :: agent %d not found" % (i,)
            raise AttributeError

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

        elif name == 'crouch_height' or name == 'c_height':
            return a.box.c_height
        elif name == 'box_height' or name == 'b_height':
            return a.box.b_height
        elif name == 'box_r':
            return a.box.box_r

        elif name == 'crouching':
            return a.s.crouching

        print 'AgentWrapper :: Couldnt find %s. There is a problem' % name
        raise AttributeError
        

class PlayerAgentWrapper(object):

    properties = ['camera_height',]

    def __init__(self, int id):
        self.id = id
        set_PlayerAgent_id(id)

    def __getattribute__(self, name):
        if name not in PlayerAgentWrapper.properties:
            raise AttributeError

        if name == 'camera_height':
            return playerAgent_state.camera_height()

        print "PlayerAgentWrapper :: couldnt find %s. There is a problem" % name
        raise AttributeError

    def update_smoothing(self, int t):
        playerAgent_state.calculate_smoothing()

    def _pos(self):
        cdef float x
        cdef float y
        cdef float z

        x = playerAgent_state.active_camera_state.x
        y = playerAgent_state.active_camera_state.y
        z = playerAgent_state.active_camera_state.z

        return [x,y,z]

    def smoothed(self):
        playerAgent_state.set_active_camera_state(smoothed)
        return self._pos()

    def client_side_prediction_interpolated(self):
        playerAgent_state.set_active_camera_state(client_side_prediction_interpolated)
        return self._pos()

    def client_side_prediction(self):
        playerAgent_state.set_active_camera_state(client_side_prediction)
        return self._pos()

    def last_snapshot(self):
        playerAgent_state.set_active_camera_state(last_snapshot)
        return self._pos()

#functions
'''
def teleport_Agent(int id, float x, float y, float z):
    cdef Agent_state* a
    a = agent_list.get(id)
    if a != NULL:
        a.teleport(x,y,z)
    else:
        print "Cannot teleport agent: agent %i does not exist" % (id,)
'''

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

