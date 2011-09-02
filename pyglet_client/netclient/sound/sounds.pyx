cdef extern from "./sound.h":
    void init_sound_system(int virtual_voices)
    void update_sound_system()
    void release_sound_system()

cdef extern from "./player_sounds.h":
    void load_player_gunshot()
    void play_player_gunshot()
    void end_player_gunshot()

def init():
    init_sound_system(100)
    load_player_gunshot()

def update():
    update_sound_system()

def play():
    play_player_gunshot()

def done():
    end_player_gunshot()
    release_sound_system()
