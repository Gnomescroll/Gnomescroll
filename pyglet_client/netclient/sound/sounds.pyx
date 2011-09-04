cdef extern from "./sound.h":
    void C_init(int virtual_voices, float vol)
    void update_sound_system()
    void release_globals()

cdef extern from "./player_sounds.h":
    void load_player_gunshot()
    void play_player_gunshot()
    void end_player_gunshot()

_enabled = True
def init(enabled=True, sfxvol=1.0, musicvol=1.0, virtual_voices=100):
    global _enabled
    _enabled = enabled
    _init(sfxvol, virtual_voices)
        
def _init(vol, virtual_voices):
    if not _enabled:
        return
    C_init(virtual_voices, vol)
    load_player_gunshot()

def update():
    if not _enabled:
        return
    update_sound_system()

def play():
    if not _enabled:
        return
    play_player_gunshot()

def done():
    if not _enabled:
        return
    end_player_gunshot()
    release_globals()
