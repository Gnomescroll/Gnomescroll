#!/usr/bin/python

from pyglet import clock
from pyglet import font
from pyglet import gl
from pyglet import graphics
from pyglet import image

from chat_client import ChatClientGlobal

'''
HUD overlay
'''

base_dir = "./"

class Hud(object):

    def __init__(self, win):
        self.win = win
        self.font = font.load('Helvetica', 14, bold=True)
        self.text = font.Text(
            self.font,
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

    def _draw_box(self):
        #draw a 180x180 red box
        graphics.draw(2, gl.GL_LINES, ('v2f\static', (20, 20, 20, 200)), ('c3B\static', (215,0,0) *2))
        graphics.draw(2, gl.GL_LINES, ('v2f\static', (20, 200, 200, 200)), ('c3B\static', (215,0,0) *2))
        graphics.draw(2, gl.GL_LINES, ('v2f\static', (200, 200, 200, 20)), ('c3B\static', (215,0,0) *2))
        graphics.draw(2, gl.GL_LINES, ('v2f\static', (200, 20, 20, 20)), ('c3B\static', (215,0,0) *2))

    def draw_chat(self):
        self._draw_chat_messages()

    def _to_draw_text(self, text='', offset=120):
        txt = font.Text(
            self.font,
            text = ChatClientGlobal.chatRender.user_input(),
            x = 20,
            y = self.win.height - offset,
            z = 0,
            color = (255,40,0,1)
        )
        return txt

    def _draw_chat_input(self, txt=None):
        if txt is None:
            txt = self._to_draw_text(ChatClientGlobal.chatRender.user_input(), 120)
        else:
            txt.text = ChatClientGlobal.chatRender.user_input()
            txt.y = self.win.height - 120
        txt.draw()
        return txt

    def _draw_chat_messages(self):
        offset = 20
        msg_height = 0
        line_height = 5
        i = 0
        txt = self._draw_chat_input()
        for msg in ChatClientGlobal.chatRender.messages():
            print 'drawing "%s"' % (msg.payload.content,)
            if txt is None:
                txt = self._to_draw_text(text=msg.payload.content, offset=offset + (line_height * i) + msg_height)
            else:
                txt.text = msg.payload.content
                txt.y = self.win.height - (offset + (line_height * i) + msg_height)
            msg_height += txt.height
            txt.draw()
            i += 1
