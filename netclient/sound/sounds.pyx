cdef extern from "./sound.h":
    void C_init(float vol)
    void update_sound_system()
    void release_all()
    void update_listener(float x, float y, float z, float vx, float vy, float vz, float fx, float fy, float fz, float ux, float uy, float uz)
    int MAX_CHANNELS
    int MAX_SOUNDS

    int load_2d_sound(char* soundfile)
    int load_3d_sound(char* soundfile, float mindistance)
    int play_2d_sound(int snd_id)
    int play_3d_sound(int snd_id, float x, float y, float z, float vx, float vy, float vz)
    void end_sound(int snd_id)
    
    int update_channel(int ch_id, float x, float y, float z, float vx, float vy, float vz)

#cdef extern from "./player_sounds.h":
#    void load_player_gunshot()
#    void play_player_gunshot()
#    void end_player_gunshot()

#cdef extern from "./projectile_sounds.h":
#    void load_projectile()
#    void end_projectile()
#    int play_projectile_sound(float x, float y, float z, float vx, float vy, float vz)
#    int update_projectile_sound(int p_snd_id, float x, float y, float z, float vx, float vy, float vz)

_enabled = True
#decorator to require enabled audio
def e(f):
    def wrapped(*args, **kwargs):
        if not _enabled:
            return
        return f(*args, **kwargs)
    return wrapped


def init(enabled=True, soundfiles=None, sfxvol=1.0, musicvol=1.0):
    global _enabled
    if soundfiles is None:
        enabled = False
    _enabled = enabled
    _init(sfxvol, soundfiles)

@e
def _init(vol, soundfiles):
    C_init(vol)
    set_soundfiles(soundfiles)

'''
Load soundfiles, and save ID for playing later

TODO:
Config to specify which sounds should be loaded and if 2D and/or 3D
e.g.
# fx_name : [filename, 2d, 3d]
{
'projectile' : ['warp2.wav', 0, 1],
'gunshot'    : ['semishoot.wav', 1, 0],
}

'''
_snd_pth = './media/sound/wav/'
_sounds_2d = {}
_sounds_3d = {}
def set_soundfiles(soundfiles):
    # just load a 2d and 3d copy for now. be specific later
    global _sounds_2d
    global _sounds_3d
    global _snd_pth
    for snd in soundfiles[0:MAX_SOUNDS/2]:
        snd_pth = _snd_pth + snd
        snd_id = load_2d_sound(snd_pth)
        _sounds_2d[snd] = snd_id
        snd_id = load_3d_sound(snd_pth, 100.0)
        _sounds_3d[snd] = snd_id

@e
def update(listener_state=None):
    if not listener_state:
        update_sound_system()
        return
    pos, vel, forw, up = listener_state
    x,y,z = pos
    vx,vy,vz = vel
    fx,fy,fz = forw
    ux,uy,uz = up
    update_listener(x,y,z, vx,vy,vz, fx,fy,fz, ux,uy,uz)
    update_sound_system()

@e
def play_2d(snd):
    global _sounds_2d
    if snd in _sounds_2d:
        play_2d_sound(_sounds_2d[snd])
    else:
        print 'Warning: %s sound file does not exist.' % (snd,)

@e
def play_3d(snd, pos, vel):
    global _sounds_3d
    x,y,z = pos
    vx,vy,vz = vel

    ch_id = -1
    if snd in _sounds_3d:
        snd_id = _sounds_3d[snd]
        ch_id = play_3d_sound(snd_id, x, y, z, vx, vy, vz)
    else:
        print 'Warning: %s sound file does not exist.' % (snd,)
    return ch_id

@e
def update_3d(ch, pos, vel):
    if ch < 0:
        return -1
    x,y,z = pos
    vx,vy,vz = vel
    fail = update_channel(ch, x,y,z, vx,vy,vz)
    if fail:
        return -1
    return ch

@e
def done():
    release_all()

#@e
#def update_projectile(p_snd_id, pos, vel):
#    if p_snd_id < 0:
#        return -1
#    x,y,z = pos
#    vx,vy,vz = vel
#    fail = update_projectile_sound(p_snd_id, x,y,z, vx,vy,vz)
#    if fail:
#        return -1
#    return p_snd_id

#@e
#def play_gunshot():
#    play_player_gunshot()

#@e
#def play_projectile(pos, vel):
#    x,y,z = pos
#    vx,vy,vz = vel
#    p_snd_id = play_projectile_sound(x,y,z, vx,vy,vz)
#    return p_snd_id

