cdef extern from "./sound.h":
    void C_init(float vol)
    void update_sound_system()
    void release_globals()
    void update_listener(float x, float y, float z, float vx, float vy, float vz, float fx, float fy, float fz, float ux, float uy, float uz)
    int MAX_SOUNDS

cdef extern from "./player_sounds.h":
    void load_player_gunshot()
    void play_player_gunshot()
    void end_player_gunshot()

cdef extern from "./projectile_sounds.h":
    void load_projectile()
    void end_projectile()
    int play_projectile_sound(float x, float y, float z, float vx, float vy, float vz)
    int update_projectile_sound(int p_snd_id, float x, float y, float z, float vx, float vy, float vz)

_enabled = True
#decorator to require enabled audio
def e(f):
    def wrapped(*args, **kwargs):
        if not _enabled:
            return
        return f(*args, **kwargs)
    return wrapped

def init(enabled=True, sfxvol=1.0, musicvol=1.0):
    global _enabled
    _enabled = enabled
    _init(sfxvol)

@e
def _init(vol):
    C_init(vol)
    load_player_gunshot()
    load_projectile()

@e
def update(pos=None, vel=None, forw=None, up=None):
    if None in [pos, vel, forw, up]:
        update_sound_system()
        return
    x,y,z = pos
    vx,vy,vz = vel
    fx,fy,fz = forw
    ux,uy,uz = up
    update_listener(x,y,z, vx,vy,vz, fx,fy,fz, ux,uy,uz)
    update_sound_system()

@e
def play_gunshot():
    play_player_gunshot()

@e
def play_projectile(pos, vel):
    x,y,z = pos
    vx,vy,vz = vel
    p_snd_id = play_projectile_sound(x,y,z, vx,vy,vz)
    return p_snd_id

@e
def update_projectile(p_snd_id, pos, vel):
    if p_snd_id < 0:
        return
    x,y,z = pos
    vx,vy,vz = vel
    fail = update_projectile_sound(p_snd_id, x,y,z, vx,vy,vz)
    if fail:
        return -1
    return p_snd_id

@e
def done():
    end_player_gunshot()
    end_projectile()
    release_globals()
