

from libcpp cimport bool

"""

cdef extern from "./options.hpp" namespace "Options":
    void set_name(char* name)
    void set_server(char* server)
    void set_port(int port)
    void set_auto_assign_team(bool auto_assign_team)
    void set_width(int width)
    void set_height(int height)
    void set_fullscreen(bool fullscreen)
    void set_fov(float fov)
    void set_sensitivity(float sensitivity)
    void set_camera_speed(float camera_speed)
    void set_invert_mouse(bool invert_mouse)
    void set_hud(bool hud)
    void set_diagnostic_hud(bool diagnostic_hud)
    void set_fps(bool fps)
    void set_ping(bool ping)
    void set_ping_update_interval(int ping_update_interval)
    void set_font(char* font)
    void set_sound(bool sound)
    void set_sfx(int sfx)
    void set_music(int music)
    void set_sound_device(char *sound_device)
    void set_animations(bool animations)
    void set_logger(bool logger)

def load_options(opts):
    set_name(opts.name)
    set_server(opts.server)
    set_port(opts.port)
    set_auto_assign_team(opts.auto_assign_team)
    set_width(opts.width)
    set_height(opts.height)
    set_fullscreen(opts.fullscreen)
    set_fov(opts.fov)
    set_sensitivity(opts.sensitivity)
    set_camera_speed(opts.camera_speed)
    set_invert_mouse(opts.invert_mouse)
    set_hud(opts.hud)
    set_diagnostic_hud(opts.diagnostic_hud)
    set_fps(opts.fps)
    set_ping(opts.ping)
    set_ping_update_interval(opts.ping_update_interval)
    set_font(opts.font)
    set_sound(opts.sound)
    set_sfx(opts.sfx)
    set_music(opts.music)
    set_sound_device(opts.sound_device)
    set_animations(opts.animations)
    set_logger(opts.logger)
"""


"""
Sound
"""
cdef extern from "./sound/sound.hpp" namespace "Sound":
    void enumerate_sound_devices()

def show_sound_devices():
    enumerate_sound_devices()

"""
Game loop
"""
cdef extern from "./main.hpp" namespace "Main":
    void init()
    int run()
def init_game():
    init()
def run_game():
    run()
