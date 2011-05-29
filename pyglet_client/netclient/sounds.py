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

def soundEffect(source):
    def outer(f):
        def wrapped(*args):
            self = args[0]
            player = media.ManagedSoundPlayer()
            player.volume = self.sfx_vol
            player.queue(source)
            player.play()
        return wrapped
    return outer
    
def musicStream(source_path):
    def outer(f):
        def wrapped(*args):
            self = args[0]
            player = media.ManagedSoundPlayer()
            player.volume = self.music_vol
            source = media.load(source_path)
            player.queue(source)
            player.play()
        return wrapped
    return outer

_sfx = {
    'build' : media.load('media/build.wav', streaming=False), # little sound byte
}

_music = {
    'red_clouds'    :   'media/red_clouds.mp3',
}

class Sounds:

    def __init__(self, settings):
        try:
            self.enabled = bool(settings.audio)
        except ValueError:
            self.enabled = False
        self.sfx_vol = settings.sfx / 100.
        self.music_vol = settings.music / 100.

    @audioSwitch
    @soundEffect(_sfx['build'])
    def build(self):
        pass

    @audioSwitch
    @musicStream(_music['red_clouds'])
    def music(self):
        pass
        
playSound = Sounds(settings) # import this
