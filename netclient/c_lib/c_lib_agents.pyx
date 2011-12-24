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
cdef extern from "./physics/vector.hpp":
    cdef struct Vector:
        float x
        float y
        float z

from libcpp cimport bool

cdef extern from "./agent/agent_status.hpp":
    cdef cppclass Agent_status:
        int health
        bool dead
        int team
        int score()
        int kills
        int deaths
        int suicides
        
#collision box
cdef extern from "./agent/agent.hpp":
    cdef struct Agent_collision_box:
        float b_height
        float c_height
        float box_r

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
 
    cdef cppclass Agent_state:
        int id
        int owner
        AgentState s
        Agent_collision_box box   #why does python need this?  This is not a PlayerAgent attribute, but from net agent...
        Agent_status status

cdef extern from "./agent/agent.hpp":
    int AGENT_MAX
    cdef cppclass Agent_list:
        void draw()
        Agent_state* get(int id)
        Agent_state* create()
        Agent_state* create(int id)
        Agent_state* get_or_create(int id)
        void destroy(int _id)
        void where()
        int get_ids()
        int* ids_in_use

cdef extern from "./agent/player_agent_action.hpp":
    cdef cppclass PlayerAgent_action:
        void hit_block()
        void fire()
        void throw_grenade()

cdef extern from "./agent/player_agent.hpp":
    cdef cppclass PlayerAgent_state:
        int agent_id
        float camera_height()
        AgentState camera_state
        void toggle_camera_mode()
        void pump_camera() #update camera
        PlayerAgent_action action
        void update_sound()

cdef extern from "./state/client_state.hpp" namespace "ClientState":
    Agent_list agent_list
    void set_control_state(int f, int b, int l, int r, int jet, int jump, int crouch, int boost, int misc1, int misc2, int misc3, float theta, float phi)
    PlayerAgent_state playerAgent_state

cdef extern from "./agent/agent_vox.hpp":
    int AGENT_PART_NUM
    cdef cppclass VoxBody:
        float vox_size
        void set_part(
            float rot_x, float rot_y, float rot_z,
            float rot_ax, float rot_ay, float rot_az,
            float anc_len, float anc_x, float anc_y, float anc_z,
            int dim_x, int dim_y, int dim_z,
            int part_num
        )
    VoxBody agent_vox_dat

def draw_agents():
    agent_list.draw()

import dat.agent_dim as dat
def load_agent_voxel_dat():
    agent_vox_dat.vox_size = dat.vosize

    for part in range(AGENT_PART_NUM):
        xdim,ydim,zdim = dat.lu1[part]
        length, ax,ay,az= dat.lu2[part]
        fx,fy,fz, nx,ny,nz = dat.lu3[part]

        agent_vox_dat.set_part(fx,fy,fz,nx,ny,nz, length,ax,ay,az, xdim,ydim,zdim, part)


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
        #'crouching',
        'health', 'dead',
        'owner',
        'team',
        'score',
        'kills',
        'deaths',
        'suicides',
    ]

    def __init__(self, int id):
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
            raise ValueError, "C Agent %d not found" % (i,)

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

        #elif name == 'crouching':
        #    return a.s.crouching

        elif name == 'health':
            return a.status.health
        elif name == 'dead':
            return a.status.dead

        elif name == 'owner':
            return a.owner

        elif name == 'team':
            return a.status.team

        elif name == 'score':
            return a.status.score()
        elif name == 'kills':
            return a.status.kills
        elif name == 'deaths':
            return a.status.deaths
        elif name == 'suicides':
            return a.status.suicides
            
        print 'AgentWrapper :: Couldnt find %s. There is a problem' % name
        raise AttributeError
        

'''

Player Agent Stuff

'''

'''
    Python should not need collision box for agent
'''

class PlayerAgentWrapper(object):

    properties = ['camera_height',]

    def __init__(self, int id):
        self.id = id

    def __getattribute__(self, name):
        if name not in PlayerAgentWrapper.properties:
            raise AttributeError

        if name == 'camera_height':
            return playerAgent_state.camera_height()

        print "PlayerAgentWrapper :: couldnt find %s. There is a problem" % name
        raise AttributeError

    def _pos(self):
        cdef float x
        cdef float y
        cdef float z

        x = playerAgent_state.camera_state.x
        y = playerAgent_state.camera_state.y
        z = playerAgent_state.camera_state.z

        return [x,y,z]

    #call before rendering to have camera state updated
    def update_camera(self):
        playerAgent_state.pump_camera()

    #toggle camera toggle_camera_mode
    def toggle_agent_camera_mode(self):
        playerAgent_state.toggle_camera_mode()

    def camera_position(self):
        z_off = playerAgent_state.camera_height()
        x = playerAgent_state.camera_state.x
        y = playerAgent_state.camera_state.y
        z = playerAgent_state.camera_state.z
        return [x, y, z+z_off]

    def hit_block(self):
        playerAgent_state.action.hit_block()

    def fire_hitscan(self):
        playerAgent_state.action.fire()

    def throw_grenade(self):
        playerAgent_state.action.throw_grenade()

    def update_sound(self):
        playerAgent_state.update_sound()
 
def set_agent_control_state(int f, int b, int l, int r, int jet, int jump, int crouch, int boost, int misc1, int misc2, int misc3, float theta, float phi):
    set_control_state(f,b,l,r,jet,jump,crouch, boost, misc1, misc2, misc3, theta,phi)

def get_player_agent_id():
    return playerAgent_state.agent_id

class AgentListWrapper:

    @classmethod
    def add(cls, int id):
        agent_list.get_or_create(id)
        return id

    @classmethod
    def remove(cls, int id):
        agent_list.destroy(id)
        return id

    @classmethod
    def ids(cls):
        cdef int n
        n = agent_list.get_ids()
        ids = []
        for k in range(n):
            ids.append(agent_list.ids_in_use[k])
        return ids
        
