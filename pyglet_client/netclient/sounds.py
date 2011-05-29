'''
Audio
'''


from pyglet import options
# must be called before importing pyglet.media
options['audio'] = ('openal', 'directsound', 'alsa', 'silent')

from pyglet import media


class Sounds:

    def __init__(self):
        self.build_sound = media.load('media/build.wav', streaming=False) # little sound byte

    def build(self):
        self.build_sound.play()

    def music(self):
        music_0 = media.load('media/red_clouds.mp3')
        music_0.play()

playSound = Sounds() # import this
