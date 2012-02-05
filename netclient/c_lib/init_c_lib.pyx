from libcpp cimport bool

"""
Init
[gameloop]
"""
cdef extern from "c_lib.hpp":
    int init_c_lib()
    void close_c_lib()

_init = 0
def init():
    global _init
    if _init == 0:
        init_c_lib()
    _init = 1

def close():
    close_c_lib()

"""
Timer
[gameloopm netout]
"""
cdef extern from "../c_lib/time/physics_timer.h":
    void _START_CLOCK()
    int _GET_TICK()
    int _GET_MS_TIME()

#DEPRECATE
def StartPhysicsTimer():
    _START_CLOCK()

#DEPRECATE
def PhysicsTimerTickCheck():
    return _GET_TICK()

#DEPRECATE
def get_time():
    return _GET_MS_TIME();

#DEPRECATE
def get_tick():
    return _GET_TICK()

def START_CLOCK():
    _START_CLOCK()

def GET_TICK():
    return _GET_TICK()

def GET_MS_TIME():
    return _GET_MS_TIME();

"""
pviz
[gameloop, input]
"""
cdef extern from "../net_lib/common/pviz.h":
    void pviz_draw(float x, float y, float z)
    void toggle_latency_unit()

def _pviz_draw(float x, float y, float z):
    pviz_draw(x,y,z)

def _toggle_latency_unit():
    toggle_latency_unit()


"""
Network
[gameloop, netclient]
"""
cdef extern from "./net_lib/host.hpp":
    void client_dispatch_network_events()
    void client_connect_to(int a, int b, int c, int d, unsigned short port)
    void flush_to_net()

def NetClientDispatchNetworkEvents():
    client_dispatch_network_events()

def NetClientConnectTo(int a, int b,int c, int d, unsigned short _port):
    client_connect_to(a, b, c, d, _port)

def NetClientFlushToNet():
    flush_to_net()

"""
Python specific network
[net_*]
"""
cdef extern from "./net_lib/export.hpp":
    ctypedef void (*PY_MESSAGE_CALLBACK)(char* buff, int n, int client_id)
    ctypedef void (*PY_CLIENT_EVENT_CALLBACK)(int client_id, int event_type)
    void set_python_net_callback_function(PY_MESSAGE_CALLBACK pt)
    void set_python_net_event_callback_function(PY_CLIENT_EVENT_CALLBACK pt)
    void send_python_net_message(char* message, int size, int client_id)
    int _get_client_id()
    int _check_connection_status()

_CLIENT_CREATION_CALLBACK = None
_CLIENT_DELETION_CALLBACK = None
_CLIENT_MESSAGE_CALLBACK = None

def register_client_creation(function):
    global _CLIENT_CREATION_CALLBACK
    _CLIENT_CREATION_CALLBACK = function

def register_client_deletion(function):
    global _CLIENT_DELETION_CALLBACK
    _CLIENT_DELETION_CALLBACK = function

def register_client_message_handling(function):
    global _CLIENT_MESSAGE_CALLBACK
    _CLIENT_MESSAGE_CALLBACK = function

def get_client_id():
    return _get_client_id()

def connected():
    return _check_connection_status()

cdef void py_net_message_callback(char* buff, int n, int client_id):
    ustring = buff[:n]
    #ustring1 = ustring2
    if(_CLIENT_MESSAGE_CALLBACK != None):
        _CLIENT_MESSAGE_CALLBACK(client_id, ustring)

def _send_python_net_message(message, int client_id):
    #print "Send python net message"
    cdef int length = len(message)
    cdef char* c_string = message
    send_python_net_message(message, length, client_id)

cdef void py_net_net_event_callback(int client_id, int event_type):
    if event_type == 0:
        print "Client connected: %i" % (client_id)
        _CLIENT_CREATION_CALLBACK(client_id)
    if event_type == 1:
        print "Client disconnected: %i" % (client_id)
        _CLIENT_DELETION_CALLBACK(client_id)
        
cpdef init_python_net():
    cdef PY_MESSAGE_CALLBACK p = py_net_message_callback
    set_python_net_callback_function(py_net_message_callback)
    print "Python net callback set"
    set_python_net_event_callback_function(py_net_net_event_callback)

"""
slimes
[input]
--
"""
cdef extern from "./monsters/monsters.hpp" namespace "Monsters":
    void test(int n)

def slime_test(int n):
    test(n)

"""
ray trace
[input]
-- Get rid of "set_active_block" logic in agent, wrap directly down to C agent method
 """
cdef extern from "ray_trace/ray_trace.h":
    int* _nearest_block(float x, float y, float z, float vx, float vy, float vz, float max_distance, int z_low, int z_high)

def nearest_block(pos, vector, float max_distance=4., int z_low=4, int z_high=3):
    cdef int* i
    cdef float x
    cdef float y
    cdef float z
    cdef float vx
    cdef float vy
    cdef float vz

    x,y,z = pos
    vx,vy,vz = vector
    
    i = _nearest_block(x,y,z, vx,vy,vz, max_distance, z_low, z_high)
    if i == NULL:
        return
    return [i[0], i[1], i[2]]

"""
sound
[sound]
-- move the init logic into C
"""
cdef extern from "./sound/sound.hpp" namespace "Sound":
    void set_volume(float vol)
    void set_enabled(int y)
    void set_sound_path(char* path)

    void load_sound(char* file)
    void update_sound()
    

class Sound(object):

    @classmethod
    def init(cls, char* sound_path, sounds=[], enabled=True, float sfxvol=1.0, float musicvol=1.0):
        if not len(sounds):
            set_enabled(0)
        if sfxvol <= 0. and musicvol <= 0.:
            set_enabled(0)

        set_enabled(int(enabled))
        set_volume(sfxvol)
        set_sound_path(sound_path)

        for snd in sounds:
            load_sound(snd)

    @classmethod
    def update(cls):
        update_sound()

"""
SDL
[gameloop]
"""

cdef extern from "./SDL/SDL_functions.h":
    int _set_resolution(int xres, int yres, int fullscreen)
    int _init_video()
    void _del_video()
    int _swap_buffers()
    int _get_ticks()

def flip():
    _swap_buffers()

def get_ticks():
    return _get_ticks()

def set_resolution(xres, yres, fullscreen = 0):
    _set_resolution(xres, yres, fullscreen)

"""
Game modes (CTF)
[chat client (sends "join team" cmd)]
"""

cdef extern from "./game/teams.hpp":
    cdef cppclass CTFTeam:  # inherits Team
        pass

cdef extern from "./game/ctf.hpp":
    cdef cppclass CTF:
        void join_team(int team)
        bool auto_assign

cdef extern from "./state/client_state.hpp" namespace "ClientState":
    CTF ctf

def join_team(int team):
    ctf.join_team(team)

"""
Options & Settings
[options]
-- this is one of the few things to keep in cython until the end
"""

def load_options(opts):
    ctf.auto_assign = opts.auto_assign_team

"""
Camera
[input, gameloop]
"""
cdef extern from "./camera/camera.hpp":
    cdef cppclass CCamera:
        float fov
        float x_size
        float y_size
        float z_near
        float z_far
        float x
        float y
        float z
        float theta
        float phi
        float xl, yl, zl
        float xu, yu, zu
        float ratio

        int type

        int is_current()

        void move(float dx, float dy, float dz)
        void set_aspect(float fov, float z_near, float z_far)
        void set_projection(float x, float y, float z, float theta, float phi)
        void set_dimensions()   # uses SDL_functions' xres & yres properties
        void set_fov(float fov)
        
    cdef enum CAMERA_TYPES:
        UNKNOWN_CAM
        AGENT_CAM
        CAMERA_CAM

    CCamera* get_agent_camera()
    CCamera* get_free_camera()
    void use_agent_camera()
    void use_free_camera()
    void update_agent_camera()
    void camera_input_update(int delta_tick, bool invert, float sensitivity)
    void world_projection()
    void hud_projection()

camera_properties = [
    'fov',
    'x_size', 'y_size',
    'z_near', 'z_far',
    'x', 'y', 'z',
    'theta', 'phi',
    'xl', 'yl', 'zl',
    'xu', 'yu', 'zu',
    'ratio'
]

camera_callback = None

cdef class Camera(object):
    cdef CCamera* camera
    cdef int active
    cdef int first_person

    def __init__(self, int first_person, name="free"):
        cdef CCamera* cam

        if name == 'free':
            cam = get_free_camera()
        elif name == 'agent':
            cam = get_agent_camera()
        else:
            print "Unknown camera name %s." % (name,)
            raise ValueError
            
        if cam == NULL:
            print "Cython camera init -- Camera is null"
            raise ValueError

        self.camera = cam
        self.active = 0
        if self.camera.is_current():
            print 'Cython camera is current camera'
            self.active = 1

        self.camera.set_dimensions()

        if name.lower() == 'free':
            cam.type = CAMERA_CAM
        elif name.lower() == 'agent':
            cam.type = AGENT_CAM
        else:
            print "Warning: Creating camera with unknown name/type %s" % (name,)
            cam.type = UNKNOWN_CAM

    def set_fov(self, float fov):
        self.camera.set_fov(fov)
            
    def set_aspect(self, float fov, float z_near, float z_far):
        self.camera.set_aspect(fov, z_near, z_far)

    def set_projection(self, float x, float y, float z, float theta, float phi):
        self.camera.set_projection(x,y,z, theta, phi)

    def move(self, float dx, float dy, float dz):
        self.camera.move(dx, dy, dz)

    def forward(self):
        return [self.camera.xl, self.camera.yl, self.camera.zl]
        
    def normal(self):
        return [self.camera.xu, self.camera.yu, self.camera.zu]
        
    def _getattribute__py(self, name):
        if name == 'x':
            return self.camera.x
        elif name == 'y':
            return self.camera.y
        elif name == 'z':
            return self.camera.z
        elif name == 'xl':
            return self.camera.xl
        elif name == 'yl':
            return self.camera.yl
        elif name == 'zl':
            return self.camera.zl
        elif name == 'xu':
            return self.camera.xu
        elif name == 'yu':
            return self.camera.yu
        elif name == 'zu':
            return self.camera.zu
            
        elif name == 'theta':
            return self.camera.theta
        elif name == 'phi':
            return self.camera.phi

        elif name == 'ratio':
            return self.camera.ratio
        elif name == 'fov':
            return self.camera.fov
        elif name == 'x_size':
            return self.camera.x_size
        elif name == 'y_size':
            return self.camera.y_size

        elif name == 'z_near':
            return self.camera.z_near
        elif name == 'z_far':
            return self.camera.z_far

        raise AttributeError

    def _setattr__py(self, k, float v):
        if k == 'x':
            self.camera.x = v
        elif k == 'y':
            self.camera.y = v
        elif k == 'z':
            self.camera.z = v
        elif k == 'xl':
            self.camera.xl = v
        elif k == 'yl':
            self.camera.yl = v
        elif k == 'zl':
            self.camera.zl = v
        elif k == 'xu':
            self.camera.xu = v
        elif k == 'yu':
            self.camera.yu = v
        elif k == 'zu':
            self.camera.zu = v
        elif k == 'theta':
            self.camera.theta = v
        elif k == 'phi':
            self.camera.phi = v

        elif k == 'ratio':
            self.camera.ratio = v
        elif k == 'fov':
            self.camera.fov = v
        elif k == 'x_size':
            self.camera.x_size = v
        elif k == 'y_size':
            self.camera.y_size = v

        elif k == 'z_near':
            self.camera.z_near = v
        elif k == 'z_far':
            self.camera.z_far = v

        raise AttributeError

class CyCamera(object):
    @classmethod
    def use_agent_camera(cls):
        use_agent_camera()
    @classmethod
    def use_free_camera(cls):
        use_free_camera()
    @classmethod
    def camera_input_update(cls, int delta_tick, bool invert, float sensitivity):
        camera_input_update(delta_tick, invert, sensitivity)
    @classmethod
    def world_projection(cls):
        world_projection()
    @classmethod
    def hud_projection(cls):
        hud_projection()
    @classmethod
    def update_agent_camera(cls):
        update_agent_camera()
    

"""
Skybox
[gameloop]
"""
cdef extern from "./skybox/skybox.hpp" namespace "Skybox":
    void render()
    void load()

def load_skybox():
    load()

def render_skybox():
    render()

"""
Animations
[gameloop, input]
"""
cdef extern from "./animations/animations.hpp" namespace "Animations":
    void agent_bleed(float x, float y, float z)
    void animations_tick()
    void animations_draw()

def AnimationTick():
    animations_tick()

def AnimationDraw():
    animations_draw()

def bleed(float x, float y, float z):
    agent_bleed(x,y,z)


"""
Agents
[chat, gameloop, netclient, netout, hud, input]
"""

cdef extern from "./physics/vector.hpp":
    cdef struct Vector:
        float x
        float y
        float z

cdef extern from "./agent/agent_status.hpp":
    unsigned int PLAYER_NAME_MAX_LENGTH
    cdef cppclass Agent_status:
        int health
        bool dead
        int team
        int score()
        unsigned int kills
        unsigned int deaths
        unsigned int suicides
        unsigned int health_max
        char* name
        unsigned int coins

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
 
    cdef cppclass Agent_state:
        int id
        AgentState s
        Agent_collision_box box
        Agent_status status
        Agent_weapons weapons

cdef extern from "./agent/agent.hpp":
    cdef cppclass Agent_list:
        Agent_state* get(int id)
        Agent_state* create(int id)
        Agent_state* get_or_create(int id)
        void destroy(int _id)
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
        void display_agent_names()
        void set_control_state(int f, int b, int l, int r, int jet, int jump, int crouch, int boost, int misc1, int misc2, int misc3, float theta, float phi)


cdef extern from "./state/client_state.hpp" namespace "ClientState":
    Agent_list agent_list
    PlayerAgent_state playerAgent_state
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
        'health',
        'health_max',
        'dead',
        'team',
        'score',
        'kills',
        'deaths',
        'suicides',
        'active_weapon',
        'name',
        'coins',
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

        elif name == 'name':
            return a.status.name

        elif name == 'coins':
            return a.status.coins
            
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

    def update_sound(self):
        playerAgent_state.update_sound()

    def display_agent_names(self):
        playerAgent_state.display_agent_names()

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
    playerAgent_state.set_control_state(f, b, l, r, jet, jump, crouch, boost, misc1, misc2, misc3, theta, phi)

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


""" Input """
ctypedef unsigned char Uint8

cdef extern from "./input/input.hpp":
    ctypedef struct MouseMotion:
        int x
        int y
        int dx
        int dy
        int button
    ctypedef struct MouseEvent:
        int x
        int y
        int button
        int state

    ctypedef struct MouseMotionAverage:
        float x
        float y

### call backs
    ctypedef int (*key_state_func)(Uint8* keystate, int numkeys)
    int _key_state_callback(key_state_func user_func, Uint8* keystate, int numkeys)

    #deprecate
    ctypedef int (*key_event_func)(char key)
    int _key_event_callback(key_event_func user_func, char key)

    ctypedef int (*mouse_motion_func)(MouseMotion ms)
    int _mouse_motion_callback(mouse_motion_func user_func, MouseMotion)

    ctypedef int (*mouse_event_func)(MouseEvent me)
    int _mouse_event_callback(mouse_event_func user_func, MouseEvent me)

    ctypedef int (*key_text_event_func)(char key, char* key_name, int event_state)
    int _key_text_event(key_text_event_func user_func, char key, char* key_name, int event_state)

    ctypedef int (*quit_event_func)()
    int _quit_event_callback(quit_event_func user_func)

    cdef extern int _get_key_state(key_state_func key_state_cb)
    cdef extern int _process_events(mouse_event_func mouse_event_cb, mouse_motion_func mouse_motion_cb, key_event_func keyboard_event_cb, key_text_event_func keyboard_text_event_cb, quit_event_func quit_event_cb)
    cdef extern int _set_text_entry_mode(int n)

    cdef MouseMotionAverage* get_mouse_render_state(int t)
    cdef int _toggle_mouse_bind()

    int _init_input()

def get_mouse_deltas(int t):
    cdef MouseMotionAverage* mm
    mm = get_mouse_render_state(t)
    return [mm.x, mm.y]

def get_key_state():
    _get_key_state(&key_state_callback)

key_text_event_callback_stack = []
mouse_event_callback_stack = []

def process_events():
    global key_text_event_callback_stack, mouse_event_callback_stack
    temp = _process_events(&mouse_event_callback, &mouse_motion_callback, &key_event_callback, &key_text_event_callback, &quit_event_callback)
    while len(key_text_event_callback_stack) != 0:
        (key, key_string, event_state) = key_text_event_callback_stack.pop(0)
        input_callback.keyboard_text_event(key, key_string, event_state)

    while len(mouse_event_callback_stack) != 0:
        me = mouse_event_callback_stack.pop(0)
        input_callback.mouse_event(*me)

def set_text_entry_mode(int n):
    temp = _set_text_entry_mode(n)

class Callback_dummy:
    def keyboard_state(self, pressed_keys):
        pass
    def keyboard_event(self, key):
        pass
    def keyboard_text_event(self, key, key_string, event_type):
        pass
    def mouse_event(self, button, state, x, y):
        pass
    def mouse_motion(self, x,y,dx,dy,button):
        pass

input_callback = Callback_dummy()

def set_input_callback(callback):
    global input_callback
    input_callback = callback
    print "Input Callback Set"

cdef int key_state_callback(Uint8* keystate, int numkeys):
    global input_callback
    pressed_keys = []
    for i in range(0, numkeys):
        if keystate[i] != 0:
            pressed_keys.append(i)
    input_callback.keyboard_state(pressed_keys)

cdef int key_event_callback(char key):
    global input_callback
    input_callback.keyboard_event(key)

cdef int key_text_event_callback(char key, char* key_name, int event_state):
    global input_callback, key_text_event_callback_stack
    key_string = key_name
    cdef bytes py_string
    py_string = key_name
    key_text_event_callback_stack.append((key, key_string, event_state))

cdef int mouse_motion_callback(MouseMotion ms):
    global input_callback
    input_callback.mouse_motion(ms.x,ms.y, ms.dx,-1*ms.dy, ms.button)

cdef int mouse_event_callback(MouseEvent me):
    global input_callback, mouse_event_callback_stack
    mouse_event_callback_stack.append((me.button, me.state, me.x, -1*me.y))

cdef int quit_event_callback():
    global input_callback, key_text_event_callback_stack
    key_text_event_callback_stack.append((9999, 'QUIT', 1))

def toggle_mouse_bind():
    return _toggle_mouse_bind()

"""
HUD
-- this is here because hud.py needs to tell it to render certain things
"""

cdef extern from "./hud/hud.hpp" namespace "Hud":
    void set_hud_draw_settings(
        bool zoom,
        bool cube_selector,
        bool inventory,
        bool help,
        bool disconnected,
        bool dead,
        bool fps,
        float fps_val,
        bool ping,
        int ping_val,
        bool player_stats,
        bool chat,
        bool chat_input,
        bool chat_cursor,
        bool scoreboard,
        bool equipment,
        int equipment_slot,
        bool compass,
        bool map
    )
    void draw_hud()
    void set_chat_message(int i, char* text, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
    void set_chat_input_string(char* text)

cdef class HUD:
    @classmethod
    def draw(cls):
        draw_hud()
    @classmethod
    def set_draw_settings(cls,
        bool zoom,
        bool cube_selector,
        bool inventory,
        bool help,
        bool disconnected,
        bool dead,
        bool fps,
        float fps_val,
        bool ping,
        int ping_val,
        bool player_stats,
        bool chat,
        bool chat_input,
        bool chat_cursor,
        bool scoreboard,
        bool equipment,
        int equipment_slot,
        bool compass,
        bool map
    ):
        set_hud_draw_settings(
            zoom,
            cube_selector,
            inventory,
            help,
            disconnected,
            dead,
            fps,
            fps_val,
            ping,
            ping_val,
            player_stats,
            chat,
            chat_input,
            chat_cursor,
            scoreboard,
            equipment,
            equipment_slot,
            compass,
            map
        )
    @classmethod
    def set_chat_message(cls, i, text, color):
        cdef unsigned char r
        cdef unsigned char g
        cdef unsigned char b
        cdef unsigned char a
        r,g,b,a = color
        set_chat_message(i, text, r,g,b,a)
    @classmethod
    def set_chat_input_string(cls, text):
        set_chat_input_string(text)

"""
Cube Selector
-- this is here because input.py needs to communicate with it
"""
cdef extern from "./hud/cube_selector.hpp" namespace "HudCubeSelector":
    cdef cppclass CubeSelector:
        int get_active_id()
        int get_active_pos()
        void set_active_id(int id)
        void set_active_pos(int pos)
        
    CubeSelector cube_selector

class HudCubeSelector:
    @classmethod
    def get_active_id(cls):
        return cube_selector.get_active_id()
    @classmethod
    def get_active_pos(cls):
        return cube_selector.get_active_pos()
    @classmethod
    def set_active_id(cls, int id):
        cube_selector.set_active_id(id)
    @classmethod
    def set_active_pos(cls, int pos):
        cube_selector.set_active_pos(pos)


"""
Font
[hud]
-- The hud depdnency is only the calling of the init() method
-- No python is dependent on this code here; its mostly doing parsing
-- It does need to add font set metadata, however
"""
cdef extern from "./hud/font.hpp" namespace "HudFont":
    int load_font(char* filename)

    void add_glyph(
        int c,
        float x, float y,
        float xoff, float yoff,
        float w, float h,
        float xadvance
    )
    void set_missing_character(int cc)

import os.path

class Font:

    font_path = "./media/fonts/"
    font_ext = ".fnt"
    png_ext = "_0.png"
    missing_character = '?'
    glyphs = {}
    info = {}
    font = None

    ready = False
    
    @classmethod
    def init(cls):
        if not os.path.exists(cls.font_path):
            print "ERROR c_lib_fonts.pyx :: cannot find font path %s" % (cls.font_path,)
            cls.ready = False
            return

        import opts
        cls.font = cls(opts.opts.font)
        cls.font.parse()
        cls.font.load()

    def __init__(self, fn):
        self.fontfile = fn
        self.pngfile = ''
        self.process_font_filename()
#        self._gen_stress()
        
    def process_font_filename(self):
        fn = self.fontfile
        fn = fn.split('.')[0]
        fn += self.font_ext
        fn = self.font_path + fn
        self.fontfile = fn
        if not os.path.exists(self.fontfile):
            print "ERROR c_lib_fonts.pyx :: cannot find font file %s in %s" % (fn, self.font_path,)
            self.ready = False
            return
        self.ready = True
            
    def parse(self):
        png = ""
        
        # parse .fnt
        with open(self.fontfile) as f:
            lines = f.readlines()
            for line in lines:
                line = line.strip()
                tags = line.split()
                name = tags[0]
                tags = dict(map(lambda a: a.split('='), tags[1:]))

                if name == 'page':
                    png = tags['file'].strip('"')
                elif name == 'info':
                    self.info.update(tags)
                    print "Font: %s" % (line,)
                elif name == 'common':
                    self.info.update(tags)
                    print "Font: %s" % (line,)
                elif name == 'chars':
                    print '%s characters in font set' % (tags['count'],)
                elif name == 'char':
                    self.glyphs[int(tags['id'])] = tags

        # process png filename
        if not png:
            png = self.fontfile + self.png_ext
        fp_png = self.font_path + png
        if not os.path.exists(fp_png):
            print "ERROR c_lib_fonts.pyx :: cannot find font png file %s in %s" % (fp_png, self.font_path,)
            self.ready = False
            return
        self.pngfile = fp_png

        self.clean_glyphs()
        self.missing_character_available()

    def add_glyphs_to_c(self):
        for char_code, glyph in self.glyphs.items():
            x = float(glyph['x'])
            y = float(glyph['y'])
            xoff = float(glyph['xoffset'])
            yoff = float(glyph['yoffset'])
            w = float(glyph['width'])
            h = float(glyph['height'])
            xadvance = float(glyph['xadvance'])
            add_glyph(char_code, x, y, xoff, yoff, w,h, xadvance)

            if char_code == ord(' '):
                add_glyph(ord('\t'), x,y, xoff, yoff, w,h, xadvance)
                
    def clean_glyphs(self):
        for kc, glyph in self.glyphs.items():
            for k,v in glyph.items():
                try:
                    glyph[k] = int(glyph[k])
                except ValueError:
                    pass

    def missing_character_available(self):
        cc = ord(self.missing_character)
        if cc not in self.glyphs:
            print "ERROR Missing character placeholder %s is not a known glyph" % (self.missing_character,)
            self.ready = False
            return False
        set_missing_character(cc)
        return True
        
    def load(self):
        if not load_font(self.pngfile):
            self.ready = False
            return
        self.add_glyphs_to_c()
        self.ready = True


"""
Client State
[gameloop]
"""

cdef extern from "./state/client_state.hpp" namespace "ClientState":
    void update_client_state()
    void draw_client_state()
    void tick_client_state()

class ClientState(object):
    @classmethod
    def update(cls):
        update_client_state()
    @classmethod
    def draw(cls):
        draw_client_state()
    @classmethod
    def tick(cls):
        tick_client_state()


