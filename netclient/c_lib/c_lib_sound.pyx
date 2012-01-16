cdef extern from "./sound/sound.hpp" namespace "Sound":
    void set_volume(float vol)
    void set_enabled(int y)
    void set_sound_path(char* path)

    void load_sound(char* file)
    void update()
    

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
        update()
