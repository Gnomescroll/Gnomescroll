#!/usr/bin/python

import settings
import SDL.hud

from chat_client import ChatClientGlobal
from input import InputGlobal
from net_client import NetClientGlobal
from game_state import GameStateGlobal

'''
HUD overlay
'''

base_dir = "./media/"

class Hud(object):

    def __init__(self):

        ## HARDCODED -- replace w/ SDL window object later
        self.win_width = 800
        self.win_height = 600

        self._init_reticle()
        self._init_text_dict()
        self._init_scoreboard()
        self._init_player_stats()
        self.fps = self._to_draw_text(
            text = '',
            x = 0,
            offset = self.win_height
        )
        self.ping = self._to_draw_text(
            text = '',
            x = 0,
            offset = self.win_height - 10
        )
        self._init_block_selector()

    def _init_reticle(self):
        if settings.pyglet:
            self.reticle = image.load(base_dir + 'texture/target.png')
            self.reticle_texture = self.reticle.get_texture()

            rh = 16.
            rw = 16.

            w = float(self.win_width)/2
            h = float(self.win_height)/2
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
        else:
            # load texture
            # center it
            # draw_functions._blit_sprite
            tex_file = '%stexture/target.png' % (base_dir,)
            self.reticle = SDL.hud.reticle(tex_file, self.win_width/2, self.win_height/2)

    def _init_block_selector(self):
        tex_file = '%stexture/block_selector_hud.png' % (base_dir,)
        self.block_selector = SDL.hud.block_selector(tex_file, self.win_width - 70, 15, scale=16)

    def _init_text_dict(self):
        offset = 20
        msg_height = 0
        line_height = 20
        msg_count = range(ChatClientGlobal.chatRender.MESSAGE_RENDER_COUNT_MAX)

        self.text_dict = dict(zip(msg_count, [self._to_draw_text('', (offset + (line_height * i) + msg_height)) for i in msg_count]))
        self.text_dict['input'] = self._to_draw_text('j', 120) # 'j' to force text height to be cached so cursor will appear properly on first load
        self.text_dict['input'].text = ''
        self.text_dict['cursor_position'] = self._to_draw_text(text='')

    def _init_player_stats(self):
        self.player_stats = self._to_draw_text(
            text = '',
            offset = self.win_height,
            x = self.win_width - 330
        )

    def _init_scoreboard(self):
        self._scoreboard_properties = ['ID', 'Name', 'Kills', 'Deaths', 'Score']
        self.scoreboard = {}
        col_width = (self.win_width * 0.75) // len(self._scoreboard_properties)
        start_x = self.win_width // 8
        i = 0
        for col_name in self._scoreboard_properties:
            self.scoreboard[col_name.lower()] = self._to_draw_text(
                text = '',
                x = start_x + (i * col_width),
                offset = (self.win_height // 8),
                color = (150, 150, 255, 255)
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

    def _format_scoreboard_plain(self, stats):
        for prop in self._scoreboard_properties:
            lprop = prop.lower()
            lines = []
            lines.append(prop + '\n')
            vals = stats[lprop]
            for val in vals:
                lines.append(str(val))
            stats[lprop] = '\n'.join(lines)
        return stats

    def draw_block_selector(self):
        self.block_selector.draw()

        bs = InputGlobal.block_selector
        w = 8
        bx, by = w,w
        space = 0

        start_x = self.win_width - 70
        start_y = ((bs.y - 1) * by) + 15
        active_x = start_x + ((bs.active % bs.x) * (bx + (2*space)))
        active_y = start_y - ((bs.active // bs.x) * (by + (2*space)))
        self._draw_border_square(active_x, active_y, w+(2*space), color=(255,255,255))

    def draw_fps(self, fps_text):
        self.fps.text = str(fps_text)
        self.fps.draw()

    def draw_ping(self, ping_text):
        self.ping.text = '%sms' % (str(ping_text),)
        self.ping.draw()

    def draw(self, fps=None, ping=None, block_selector=False):
        self.draw_reticle()
        self.draw_chat()
        self.draw_player_stats()
        if InputGlobal.scoreboard:
            self.draw_scoreboard()
        if fps is not None:
            self.draw_fps(fps)
        if ping is not None:
            self.draw_ping(ping)
        if block_selector:
            self.draw_block_selector()

    def _format_player_stats_html(self):
        agent = GameStateGlobal.agent
        if agent is None:
            s = 'No agent yet'
        elif agent.team.is_viewers():
            s = 'Viewer Mode'
        else:
            health = '%i/%i' % (agent.health, agent.HEALTH_MAX,)
            weapon = agent.weapons.active()
            if weapon is not None:
                s = 'HP %s :: Ammo %s' % (health, weapon.hud_display(),)
            else:
                s = 'HP %s :: No weapon equipped' % (health,)
        s = '<font face="Monospace" color="green"><b>%s</b></font>' % (s,)
        return s

    def _format_player_stats_plain(self):
        agent = GameStateGlobal.agent
        if agent is None:
            s = 'No agent yet.'
        else:
            if agent.team is not None and agent.team.is_viewers():
                s = 'Viewer Mode'
            else:
                health = '%i/%i' % (agent.health, agent.HEALTH_MAX,)
                weapon = agent.weapons.active()
                if weapon is None:
                    w = 'No weapon equipped'
                else:
                    w = 'Ammo %s' % (weapon.hud_display(),)
                s = 'HP %s::%s' % (health, w,)
        return s

    def draw_player_stats(self):
        # draw label in top
        #stats = self._format_player_stats_html()
        stats = self._format_player_stats_plain()
        old = self.player_stats.text
        if old != stats:
            #self.player_stats.begin_update()
            self.player_stats.text = stats
            #self.player_stats.end_update()
        self.player_stats.draw()

    #def draw_reticle(self):
        #gl.glEnable(gl.GL_TEXTURE_2D)        # typically target is GL_TEXTURE_2D
        #gl.glBindTexture(self.reticle_texture.target, self.reticle_texture.id)

        #gl.glEnable(gl.GL_BLEND)
        #gl.glBlendFunc(gl.GL_SRC_ALPHA, gl.GL_ONE_MINUS_SRC_ALPHA)
        #self.reticleVertexList.draw(gl.GL_QUADS)
        #gl.glDisable(gl.GL_BLEND)

    def draw_reticle(self):
        self.reticle.draw()
        self._draw_reference_center()

    def _draw_reference_center(self):
        w = 2
        x = (self.win_width/2) - w/2
        y = (self.win_height/2) - w/2
        self._draw_square(x, y, w, color=(255,10,10))

    def draw_scoreboard(self):
        #stats_txt = self._format_scoreboard_html(GameStateGlobal.scoreboard())
        stats_txt = self._format_scoreboard_plain(GameStateGlobal.scoreboard())
        for key, txt in stats_txt.items():
            curr_sb = self.scoreboard[key]
            old = curr_sb.text
            if old != txt:
                #curr_sb.begin_update()
                curr_sb.text = txt
                #curr_sb.end_update()
            curr_sb.draw()

    def _draw_line(self, x, y, x1, y1, color=None):
        if color is None:
            color = (255, 255, 0)
        r, g, b = color
        SDL.gl.draw_line(r, g, b, x, y, 0, x1, y1, 0)

    def _draw_square(self, x, y, w, color=None):
        self._draw_rect(x, y, w, w, color)

    def _draw_rect(self, x, y, w, h, color=None):
        if color is None:
            color = (255,255,255)
        r,g,b = color
        SDL.gl.draw_rect(r,g,b, x,y, w,h)

    def _draw_border_square(self, x, y, w, color=None):
        self._draw_border_rect(x, y, w, w, color)

    def _draw_border_rect(self, x, y, w, h, color=None):
        if color is None:
            color = (255,255,255)
        r,g,b = color
        SDL.gl.draw_border_rect(r,g,b, x,y, w,h)

    def _draw_horizontal_line(self, x, y, length=10):
        self._draw_line(x, y, x + length, y)

    def _draw_vertical_line(self, x, y, length=10):
        self._draw_line(x, y, x, y + length)

    def _draw_vertical_lines(self, x, y, length=10, w=2):
        for i in range(w):
            _x = x + i
            self._draw_vertical_line(_x, y, length)

    def draw_chat(self):
        self._draw_chat_messages()
        if InputGlobal.input == 'chat':
            self._draw_chat_input(draw=True)
            self._draw_cursor()

    def _to_draw_text(self, text='', offset=120, x=20, color=(255,40,0,255)):
        txt = SDL.hud.text(
            text = text,
            x = x,
            y = self.win_height - offset,
            color = color
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
        #length = input.height / 2
        length = 3
        y = input.y
        x = input.x
        if cursor != 0:
            # this is a guess
            x += cursor * 10

            # this is the right way, if the SDL.text object calculated it's width and after __setitem__('text')
            #if cursor == input_length:
                #x += input.width
            #else:
                #dummy = self.text_dict['cursor_position']
                #dummy.text = input.text[0:cursor]
                #x += dummy.width
        self._draw_vertical_lines(x, y, length, 2)
