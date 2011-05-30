'''
Audio
'''

import settings
from pyglet import options
# must be called before importing pyglet.media

# move this settings pre-processing to another file
if not hasattr(settings, 'audio'):
    settings.audio = False
if not hasattr(settings, 'sfx'):
    settings.sfx = 10
if not hasattr(settings, 'music'):
    settings.sfx = 10

if settings.audio:
    options['audio'] = ('openal', 'directsound', 'alsa', 'silent',)
else:
    options['audio'] = ('silent',)

from pyglet import media


# decorator for toggling audio playback
def audioSwitch(f):
    def wrapped(*args):
        self = args[0]
        if self.enabled:
            f(*args)
        return args
    return wrapped

# decorator for sound effect. accepts a cached source (see _sfx for example)
def soundEffect(sfx_name):
    def outer(f):
        def wrapped(*args):
            f(*args)
            self = args[0]
            player = media.ManagedSoundPlayer()
            player.volume = self.sfx_vol
            player.queue(_sounds['sfx'][sfx_name])
            player.play()
        return wrapped
    return outer

# decorator for long/infrequent audio file (not cached)
# use for playback that isnt latency critical
def musicStream(audio_name):
    def outer(f):
        def wrapped(*args):
            f(*args)
            self = args[0]
            player = media.ManagedSoundPlayer()
            player.volume = self.music_vol
            source = media.load(_sounds['music'][audio_name])
            player.queue(source)
            player.play()
        return wrapped
    return outer


# static sound effect sources; these are cached
_sounds = {
    'sfx' : {
        'build' : 'media/build.wav', # little sound byte
    },

    # music streams (won't be cached, arent latency critical). just supply pathname
    'music' : {
        'red_clouds'    :   'media/red_clouds.mp3',
    }
}

# Add @audioSwitch for on/off ability;
# add @soundEffect for sfx, @musicStream for music
# function can just pass
# function will be called before player begins
class Sounds:

    def __init__(self, settings):
        try:
            self.enabled = bool(settings.audio)
        except ValueError:
            self.enabled = False

        if self.enabled:
            self._load_sounds()
            
        self.sfx_vol = settings.sfx / 100.
        self.music_vol = settings.music / 100.

    def _load_sounds(self):
        sfx = _sounds['sfx']
        for name, fp in sfx.items():
            sfx[name] = media.load(fp, streaming=False)
            

    @audioSwitch
    @soundEffect('build')
    def build(self):
        pass

    @audioSwitch
    @musicStream('red_clouds')
    def music(self):
        pass
        
playSound = Sounds(settings) # import this
