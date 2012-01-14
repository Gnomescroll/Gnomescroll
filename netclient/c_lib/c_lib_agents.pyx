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
        unsigned int kills
        unsigned int deaths
        unsigned int suicides
        unsigned int health_max

cdef extern from "./agent/agent_weapons.hpp":
    cdef cppclass Agent_weapons:
        int active
        char* hud_display()
        void set_active_block(int block)
        bool can_zoom()
        
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
        AgentState s
        Agent_collision_box box   #why does python need this?  This is not a PlayerAgent attribute, but from net agent...
        Agent_status status
        Agent_weapons weapons

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
        void fire()
        void reload()
        void switch_weapon(int i)

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
    cdef enum AGENT_BODY_PARTS:
        AGENT_PART_HEAD
        AGENT_PART_TORSO
        AGENT_PART_LARM
        AGENT_PART_RARM
        AGENT_PART_LLEG
        AGENT_PART_RLEG

    cdef cppclass VoxBody:
        float vox_size
        void set_part(
            float rot_x, float rot_y, float rot_z,
            float rot_ax, float rot_ay, float rot_az,
            float anc_len, float anc_x, float anc_y, float anc_z,
            int dim_x, int dim_y, int dim_z,
            int part_num
        )
        void set_color(int part, int x, int y, int z, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
    VoxBody agent_vox_dat

def draw_agents():
    agent_list.draw()


#head=\
#{"voxels":[[1,4,1,48,48,48,255],[4,4,1,48,48,48,255],[3,4,1,48,48,48,255],[2,4,1,48,48,48,255],[0,2,1,48,48,48,255],[2,0,1,48,48,48,255],[1,0,1,48,48,48,255],[3,0,1,48,48,48,255],[4,0,1,48,48,48,255],[5,1,1,48,48,48,255],[5,3,1,48,48,48,255],[5,3,2,48,48,48,255],[5,1,2,48,48,48,255],[4,0,2,48,48,48,255],[3,0,2,48,48,48,255],[2,0,2,48,48,48,255],[1,0,2,48,48,48,255],[0,1,1,48,48,48,255],[0,3,1,48,48,48,255],[1,4,2,48,48,48,255],[2,4,2,48,48,48,255],[3,4,2,48,48,48,255],[4,4,2,48,48,48,255],[0,2,2,48,48,48,255],[0,3,2,127,31,223,255],[0,1,2,127,31,223,255],[0,1,3,48,48,48,255],[0,2,3,48,48,48,255],[0,3,3,48,48,48,255],[4,3,3,48,48,48,255],[4,3,0,48,48,48,255],[4,2,0,48,48,48,255],[4,1,0,48,48,48,255],[3,1,0,48,48,48,255],[3,2,0,48,48,48,255],[3,3,0,48,48,48,255],[2,3,0,48,48,48,255],[2,2,0,48,48,48,255],[2,1,0,48,48,48,255],[1,1,0,48,48,48,255],[1,3,0,48,48,48,255],[1,2,0,31,223,223,255],[4,1,3,48,48,48,255],[3,1,3,48,48,48,255],[3,3,3,48,48,48,255],[2,3,3,48,48,48,255],[2,1,3,48,48,48,255],[1,1,3,48,48,48,255],[1,3,3,48,48,48,255],[1,2,3,223,31,31,255],[2,2,3,223,31,31,255],[3,2,3,223,31,31,255],[4,2,3,223,31,31,255],[5,2,2,223,31,31,255],[5,2,1,223,31,31,255],[1,2,4,223,31,31,255],[2,2,4,223,31,31,255],[3,2,4,223,31,31,255],[4,2,4,223,31,31,255],[5,2,3,223,31,31,255],[6,2,2,223,31,31,255],[6,2,1,223,31,31,255],[5,2,0,223,31,31,255],[0,2,0,31,223,223,255],[0,1,0,48,48,48,255],[0,3,0,48,48,48,255]],"vosize":0.2,"dim":[7,5,5]}

#head=\
#{"voxels":[[2,0,0,223,31,31,255],[0,0,0,223,31,31,255],[1,0,0,223,31,31,255],[1,0,1,31,223,223,255]],"vosize":0.2,"dim":[3,1,2]}

head=\
{"voxels":[[0,0,0,48,48,48,255],[1,0,0,48,48,48,255],[2,0,0,48,48,48,255],[3,0,0,48,48,48,255],[0,1,0,31,223,80,255],[0,2,0,31,223,80,255],[0,3,0,31,223,80,255],[0,0,1,223,175,31,255],[0,0,2,223,175,31,255],[0,0,3,223,175,31,255]],"vosize":0.2,"dim":[4,4,4]}

import dat.agent_dim as dat
def load_agent_voxel_dat():
    agent_vox_dat.vox_size = dat.vosize

    for part in range(AGENT_PART_NUM):
        if part == AGENT_PART_HEAD:
            xdim,ydim,zdim = head['dim']
        else:
            xdim,ydim,zdim = dat.lu1[part]
            
        length, ax,ay,az= dat.lu2[part]
        fx,fy,fz, nx,ny,nz = dat.lu3[part]

        agent_vox_dat.set_part(fx,fy,fz,nx,ny,nz, length,ax,ay,az, xdim,ydim,zdim, part)
        if part == AGENT_PART_HEAD:
            for vox in head['voxels']:
                x,y,z, r,g,b,a = vox
                agent_vox_dat.set_color(part, x,y,z, r,g,b,a)

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
        'health',
        'health_max',
        'dead',
        'team',
        'score',
        'kills',
        'deaths',
        'suicides',
        'active_weapon',
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
        elif name == 'health_max':
            return a.status.health_max
        elif name == 'dead':
            return a.status.dead

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

        elif name == 'active_weapon':
            return a.weapons.active
            
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

    def switch_weapon(self, int i):
        playerAgent_state.action.switch_weapon(i)

    def fire(self):
        playerAgent_state.action.fire()

#    def hit_block(self):
#        playerAgent_state.action.hit_block()

#    def fire_hitscan(self):
#        playerAgent_state.action.fire()

#    def throw_grenade(self):
#        playerAgent_state.action.throw_grenade()

    def update_sound(self):
        playerAgent_state.update_sound()

    def weapon_hud_display(self):
        cdef Agent_state* a
        a = agent_list.get(playerAgent_state.agent_id)
        if a == NULL:
            return "No agent yet"
        return a.weapons.hud_display()

    def reload(self):
        playerAgent_state.action.reload()

    def set_active_block(self, int block_id):
        cdef Agent_state* a
        a = agent_list.get(playerAgent_state.agent_id)
        if a == NULL:
            return
        a.weapons.set_active_block(block_id)

    def can_zoom(self):
        cdef Agent_state* a
        a = agent_list.get(playerAgent_state.agent_id)
        if a == NULL:
            return False
        return a.weapons.can_zoom()
        
 
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
