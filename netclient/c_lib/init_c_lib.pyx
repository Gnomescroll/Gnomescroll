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
sound
[sound]
-- needs a listdir (or hardcode each soundfile [no]) to move into C
-- dirent is probably cross platform
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
cdef extern from "./game/ctf.hpp":
    cdef cppclass CTF:
        void join_team(int team)
        bool auto_assign

cdef extern from "./state/client_state.hpp" namespace "ClientState":
    CTF ctf
    void update_camera(int delta_tick)

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
[gameloop]
"""
cdef extern from "./camera/camera.hpp":
    void world_projection()
    void hud_projection()

camera_callback = None
def camera_world_projection():
    world_projection()
def camera_hud_projection():
    hud_projection()
def update_camera_state(int delta_tick):
    update_camera(delta_tick)
    
"""
Animations
[gameloop]
"""
cdef extern from "./animations/animations.hpp" namespace "Animations":
    void animations_tick()
    void animations_draw()

def AnimationTick():
    animations_tick()

def AnimationDraw():
    animations_draw()

"""
Agents
[chat, gameloop, netclient, netout, hud]
"""

cdef extern from "./agent/agent_status.hpp":
    unsigned int PLAYER_NAME_MAX_LENGTH
    cdef cppclass Agent_status:
        char* name

cdef extern from "./agent/agent.hpp":
    cdef cppclass Agent_state:
        int id
        Agent_status status

cdef extern from "./agent/agent.hpp":
    cdef cppclass Agent_list:
        Agent_state* get(int id)
        Agent_state* get_or_create(int id)
        int get_ids()
        int* ids_in_use

cdef extern from "./agent/player_agent.hpp":
    cdef cppclass PlayerAgent_state:
        int agent_id
        void update_sound()
        void display_agent_names()
        bool identified
        Agent_state* you


cdef extern from "./state/client_state.hpp" namespace "ClientState":
    Agent_list agent_list
    PlayerAgent_state playerAgent_state
    int get_client_id_from_name(char* name)

def update_sound_listener():
    playerAgent_state.update_sound()

def display_agent_names():
    playerAgent_state.display_agent_names()

def get_player_agent_id():
    return playerAgent_state.agent_id

def client_id_from_name(name):
    get_client_id_from_name(name)

def player_agent_assigned():
    if playerAgent_state.you != NULL:
        return True
    return False

def get_agent_name(int id):
    cdef Agent_state* a
    a = agent_list.get(id)
    if a == NULL: return ''
    return a.status.name


""" Chat """

cdef extern from "./input/handlers.hpp":
    int CHAT_BUFFER_SIZE
    int* chat_input_buffer_unicode
    char** chat_input_buffer_sym
    int chat_cursor_index
    void clear_chat_buffer()

def get_chat_input_buffer():
    sym_buff = []
    uni_buff = []
    for i in range(chat_cursor_index):
        sym = chat_input_buffer_sym[i]
        try:
            uni = unichr(chat_input_buffer_unicode[i])
        except:
            uni = sym
        sym_buff.append(sym)
        uni_buff.append(uni)
    return sym_buff, uni_buff

def clear_chat_input_buffer():
    clear_chat_buffer()
    
""" Input """

cdef extern from "./input/input.hpp":
    int get_key_state()
    int process_events()

def process_input():
    process_events()
    get_key_state()

cdef extern from "./input/handlers.hpp":

    cdef enum InputStateMode:
        INPUT_STATE_AGENT
        INPUT_STATE_CAMERA

    cdef struct InputState:
        bool chat
        bool quit
        InputStateMode input_mode
        InputStateMode camera_mode

    InputState input_state

    void set_input_options(
        bool invert_mouse,
        float sensitivity
    )

class CyInputState(object):
    def __setattr__(self,k,v):
        if k == 'chat':
            input_state.chat = v
        elif k == 'quit':
            input_state.quit = v
        elif k == 'input_mode':
            input_state.input_mode = v
        elif k == 'camera_mode':
            input_state.camera_mode = v
        else:
            raise AttributeError
    def __getattribute__(self, k):
        if k == 'chat':
            return input_state.chat
        elif k == 'quit':
            return input_state.quit
        elif k == 'input_mode':
            return input_state.input_mode
        elif k == 'camera_mode':
            return input_state.camera_mode
        else:
            return object.__getattribute__(self, k)
    def set_options(self,
        bool invert_mouse,
        float sensitivity
    ):
        set_input_options(invert_mouse, sensitivity)
    
cy_input_state = CyInputState()

"""
HUD
-- this is here because hud.py needs to tell it to render certain things
"""

cdef extern from "./hud/hud.hpp" namespace "Hud":
    void set_hud_draw_settings(
        bool connected,
        bool fps,
        float fps_val,
        bool ping,
        int ping_val,
    )
    void draw_hud()
    void set_chat_message(int i, char* text, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
    void set_chat_input_string(char* text)
    void update_hud_draw_settings()

cdef class HUD:
    @classmethod
    def draw(cls):
        draw_hud()
    @classmethod
    def set_draw_settings(cls,
        bool connected,
        bool fps,
        float fps_val,
        bool ping,
        int ping_val,
    ):
        set_hud_draw_settings(
            connected,
            fps,
            fps_val,
            ping,
            ping_val,
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
    @classmethod
    def update_hud_draw_settings(cls):
        update_hud_draw_settings()

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

    void send_identify_packet(char* name)

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

def identify(name):
    send_identify_packet(name)
def identified():
    return playerAgent_state.identified

