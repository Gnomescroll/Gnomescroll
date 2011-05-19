#!/usr/bin/python

from pyglet import clock
from pyglet import font
from pyglet import gl
from pyglet import graphics
from pyglet import image

'''
HUD overlay
'''

base_dir = "./"

class Hud(object):

    def __init__(self, win):
        self.win = win
        helv = font.load('Helvetica', win.width / 15.0)
        self.text = font.Text(
            helv,
            'Hello, World!',
            x=win.width / 2,
            y=win.height / 2,
            halign=font.Text.CENTER,
            valign=font.Text.CENTER,
            color=(1, 1, 1, 0.5),
        )
        self.fps = clock.ClockDisplay()
        self._init_reticle()

    def _init_reticle(self):
        self.reticle = image.load(base_dir + 'texture/target.png')
        self.reticle_texture = self.reticle.get_texture()

        rh = 16.
        rw = 16.

        w = float(self.win.width)/2
        h = float(self.win.height)/2
        print str((h,w))
        x_min = w - rw/2
        x_max = w + rw/2
        y_min = h - rh/2
        y_max = h + rh/2

        v_list = [
            x_min, y_max,
            x_max, y_max,
            x_max, y_min,
            x_min, y_min
        ]
        print str(v_list)

        self.reticleVertexList = graphics.vertex_list(
            4,
            ('v2f\static', v_list),
            ('c3B\static', (255,255,255) *4),
            ("t3f\static", self.reticle_texture.tex_coords)
        )

    def draw(self):
        #self.text.draw()
        self.draw_reticle()
        self.draw_chat()
        self.fps.draw()

    def draw_reticle(self):
        gl.glEnable(gl.GL_TEXTURE_2D)        # typically target is GL_TEXTURE_2D
        gl.glBindTexture(self.reticle_texture.target, self.reticle_texture.id)

        gl.glEnable(gl.GL_BLEND)
        gl.glBlendFunc(gl.GL_SRC_ALPHA, gl.GL_ONE_MINUS_SRC_ALPHA)
        self.reticleVertexList.draw(gl.GL_QUADS)
        gl.glDisable(gl.GL_BLEND)

    def draw_chat(self):
        #draw a box
        graphics.draw(2, gl.GL_LINES, ('v2f\static', (20, 20, 20, 200)), ('c3B\static', (255,255,255) *4))
        graphics.draw(2, gl.GL_LINES, ('v2f\static', (20, 200, 200, 200)), ('c3B\static', (255,255,255) *4),)
        graphics.draw(2, gl.GL_LINES, ('v2f\static', (200, 200, 200, 20)), ('c3B\static', (255,255,255) *4),)
        graphics.draw(2, gl.GL_LINES, ('v2f\static', (200, 20, 20, 20)), ('c3B\static', (255,255,255) *4),)
