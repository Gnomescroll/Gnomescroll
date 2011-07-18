#!/usr/bin/python

from pyglet import clock
from pyglet import font
from pyglet import gl
from pyglet import graphics
from pyglet import image
from pyglet import text

from chat_client import ChatClientGlobal
from input import InputGlobal
from net_client import NetClientGlobal
from game_state import GameStateGlobal

'''
HUD overlay
'''

base_dir = "./"

class Hud(object):

    def __init__(self, win):
        self.win = win
        self.font = font.load('Helvetica', 14, bold=True)
        self.text = self._to_draw_text()
        self.fps = clock.ClockDisplay()
        self._init_reticle()
        self._init_text_dict()
        self._init_scoreboard()
        self._init_player_stats()

    def _init_text_dict(self):
        offset = 20
        msg_height = 0
        line_height = 20
        msg_count = ChatClientGlobal.chatRender.MESSAGE_RENDER_COUNT_MAX
        self.text_dict = dict(zip([i for i in range(msg_count)], [self._to_draw_text('', (offset + (line_height * i) + msg_height)) for i in range(msg_count)]))
        self.text_dict['input'] = self._to_draw_text('j', 120) # 'j' to force text height to be cached so cursor will appear properly on first load
        self.text_dict['input'].text = ''
        self.text_dict['cursor_position'] = self._to_draw_text(text='')

    def _init_player_stats(self):
        self.player_stats = text.HTMLLabel(
            text = '',
            x = self.win.width - 10,
            y = 10,
            anchor_x = 'right',
            anchor_y = 'center',
        )
        self.player_stats.font_size = 8

    def _init_scoreboard(self):
        self._scoreboard_properties = ['ID', 'Name', 'Kills', 'Deaths', 'Score']
        self.scoreboard = {}
        col_width = (self.win.width * 0.75) // len(self._scoreboard_properties)
        start_x = self.win.width // 8
        i = 0
        for col_name in self._scoreboard_properties:
            self.scoreboard[col_name.lower()] = text.HTMLLabel(
                text = '',
                x = start_x + (i * col_width),
                y = (self.win.height // 8) * 7,
                anchor_x = 'center',
                anchor_y = 'center',
                multiline = True,
                width = col_width
            )
            i += 1

    def _format_scoreboard_html(self, stats):
        for prop in self._scoreboard_properties:
            lprop = prop.lower()
            lines = []
            lines.append(prop + '<br>')

            vals = stats[lprop]
            for val in vals:
                lines.append(str(val))
            stats[lprop] = '<font face="Times New Roman" size="15" color="red">%s</font>' % ('<br>'.join(lines),)

        return stats

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
        self.draw_player_stats()
        if InputGlobal.scoreboard:
            self.draw_scoreboard()
        self.fps.draw()

    def _format_player_stats_html(self):
        agent = GameStateGlobal.agent
        if agent is None:
            s = 'No agent yet'
        else:
            health = '%i/%i' % (agent.health, agent.HEALTH_MAX,)
            weapon = agent.weapons.active()
            if weapon is not None:
                s = 'HP %s :: Ammo %s' % (health, weapon.hud_display(),)
            else:
                s = 'HP %s :: No weapon equipped' % (health,)
        s = '<font face="Monospace" color="green"><b>%s</b></font>' % (s,)
        return s

    def draw_player_stats(self):
        print 'player stats'
        # draw label in top
        stats = self._format_player_stats_html()
        old = self.player_stats.text
        if old != stats:
            self.player_stats.begin_update()
            self.player_stats.text = stats
            self.player_stats.end_update()
        self.player_stats.draw()

    def draw_reticle(self):
        gl.glEnable(gl.GL_TEXTURE_2D)        # typically target is GL_TEXTURE_2D
        gl.glBindTexture(self.reticle_texture.target, self.reticle_texture.id)

        gl.glEnable(gl.GL_BLEND)
        gl.glBlendFunc(gl.GL_SRC_ALPHA, gl.GL_ONE_MINUS_SRC_ALPHA)
        self.reticleVertexList.draw(gl.GL_QUADS)
        gl.glDisable(gl.GL_BLEND)

    def draw_scoreboard(self):
        stats_txt = self._format_scoreboard_html(GameStateGlobal.scoreboard())
        for key, txt in stats_txt.items():
            curr_sb = self.scoreboard[key]
            old = curr_sb.text
            if old != txt:
                curr_sb.begin_update()
                curr_sb.text = txt
                curr_sb.end_update()
            curr_sb.draw()

    def _draw_box(self):
        #draw a 180x180 red box
        graphics.draw(2, gl.GL_LINES, ('v2f\static', (20, 20, 20, 200)), ('c3B\static', (215,0,0) *2))
        graphics.draw(2, gl.GL_LINES, ('v2f\static', (20, 200, 200, 200)), ('c3B\static', (215,0,0) *2))
        graphics.draw(2, gl.GL_LINES, ('v2f\static', (200, 200, 200, 20)), ('c3B\static', (215,0,0) *2))
        graphics.draw(2, gl.GL_LINES, ('v2f\static', (200, 20, 20, 20)), ('c3B\static', (215,0,0) *2))

    def _draw_horizontal_line(self, x, y, length=10):
        x1 = x + length
        y1 = y
        graphics.draw(2, gl.GL_LINES, ('v2f\static', (x, y, x1, y1)), ('c3B\static', (255, 255, 0) *2))
        graphics.draw(2, gl.GL_LINES, ('v2f\static', (x, y+1, x1, y1+1)), ('c3B\static', (255, 255, 0) *2))

    def _draw_vertical_line(self, x, y, length=10):
        x1 = x
        y1 = y + length
        graphics.draw(2, gl.GL_LINES, ('v2f\static', (x, y, x1, y1)), ('c3B\static', (255, 255, 0) *2))
        graphics.draw(2, gl.GL_LINES, ('v2f\static', (x+1, y, x1+1, y1)), ('c3B\static', (255, 255, 0) *2))

    def draw_chat(self):
        self._draw_chat_messages()
        if InputGlobal.input == 'chat':
            self._draw_chat_input(draw=True)
            self._draw_cursor()
        gl.glDisable(gl.GL_BLEND)

    def _to_draw_text(self, text='', offset=120):
        txt = font.Text(
            self.font,
            text = text,
            x = 20,
            y = self.win.height - offset,
            z = 0,
            color = (255, 40, 0, 1)
        )
        return txt

    def _draw_chat_input(self, draw=False, txt=None):
        input = self.text_dict['input']
        txt = txt or ChatClientGlobal.chatRender.user_input()
        input.text = txt
        if draw:
            input.draw()

    def _draw_chat_messages(self):
        to_draw = []
        offset = 20
        msg_height = 0
        line_height = 20
        i = 0
        msgs = ChatClientGlobal.chatRender.messages()
        pm_channel = 'pm_' + NetClientGlobal.client_id
        for msg in msgs:
            content = msg.name
            if content:
                content += ': '
            content += msg.payload.content
            channel = msg.payload.channel
            if channel == 'system':
                color = (40, 255, 0, 1)
            elif channel == pm_channel:
                color = (120, 200, 200, 1)
            else:
                color = (255, 40, 0, 1)
            txt = self.text_dict[i]
            if txt.text != content:
                txt.text = content
            if txt.color != color:
                txt.color = color
            to_draw.append(txt)
            i += 1

        for t in to_draw:
            t.draw()

    def _draw_cursor(self, direction='vertical'):
        if direction == 'vertical':
            self._draw_vertical_cursor()
        else:
            self._draw_horizontal_cursor()

    def _draw_horizontal_cursor(self):
        cursor = ChatClientGlobal.chatRender.cursor_position()
        input = self.text_dict['input']
        input_length = len(input.text)
        if input_length == 0:
            length = 10
        else:
            length = input.width / input_length
        y = input.y - 5
        x = input.x + (length * (cursor))
        self._draw_horizontal_line(x, y, length)

    def _draw_vertical_cursor(self):
        cursor = ChatClientGlobal.chatRender.cursor_position()
        input = self.text_dict['input']
        input_length = len(input.text)
        length = input.height / 2
        y = input.y
        x = input.x
        if cursor != 0:
            if cursor == input_length:
                x += input.width
            else:
                dummy = self.text_dict['cursor_position']
                dummy.text = input.text[0:cursor]
                x += dummy.width
        self._draw_vertical_line(x, y, length)
