'''
Audio
'''

import settings
from pyglet import options
# must be called before importing pyglet.media

# move this settings pre-processing to another file
if not hasattr(settings, 'AUDIO'):
    settings.AUDIO = False

if settings.AUDIO:
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
    return wrapped

class Sounds:

    def __init__(self, settings):
        try:
            self.enabled = bool(settings.AUDIO)
        except ValueError:
            self.enabled = False
            
        self.build_sound = media.load('media/build.wav', streaming=False) # little sound byte

    @audioSwitch
    def build(self):
        self.build_sound.play()

    @audioSwitch
    def music(self):
        music_0 = media.load('media/red_clouds.mp3')
        music_0.play()

playSound = Sounds(settings) # import this
