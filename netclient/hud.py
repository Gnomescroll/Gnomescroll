import opts
opts = opts.opts

import init_c_lib

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

        init_c_lib.Font.init()

        self.win_width = opts.width
        self.win_height = opts.height
        self.font_height = opts.font_size
        self.height_margin = 5
        self.width_margin = 3

        self.init_scoreboard()

    def text(self, text='', offset=120, x=20, color=(255,40,0,255)):
        txt = init_c_lib.CyText(
            text = text,
            x = x,
            y = self.win_height - offset,
            color = color
        )
        return txt

    def init_scoreboard(self):
        self.scoreboard_properties = ['ID', 'Name', 'Kills', 'Deaths', 'Score']
        self.scoreboard = {}
        col_width = (self.win_width * 0.75) // len(self.scoreboard_properties)
        start_x = self.win_width // 8
        i = 0
        for col_name in self.scoreboard_properties:
            self.scoreboard[col_name.lower()] = self.text(
                text = '',
                x = start_x + (i * col_width),
                color = (150, 150, 255, 255)
            )
            i += 1

        self.team_names = {
            1 : self.text(
                text = '',
                x = start_x,
                offset = (self.win_height // 8),
                color = (150, 150, 255, 255)
            ),
            2 : self.text(
                text = '',
                x = start_x,
                offset = (self.win_height // 8),
                color = (150, 150, 255, 255)
            )
        }

    def format_scoreboard(self, stats):
        for prop in self.scoreboard_properties:
            lprop = prop.lower()
            lines = []
            lines.append(prop + '\n')
            vals = stats[lprop]
            for i, val in enumerate(vals):
                if i in stats['team']:
                    lines.append('\n')
                lines.append(str(val))
            stats[lprop] = '\n'.join(lines)

        return stats

    def scoreboard_stats(self):
        props = ['name', 'kills', 'deaths', 'score', 'id']
        defs =  [[] for i in range(len(props))]
        stats = dict(zip(props, defs))
        stats['team'] = {}

        agents = GameStateGlobal.agentList.values()

        agents.sort(key=lambda a: a.team.id)

        curr_team = 0
        for i, agent in enumerate(agents):
            if agent.team.id and agent.team.id != curr_team:
                curr_team = agent.team.id
                stats['team'][i] = agent.team
            stats['name'].append(agent.name)
            stats['kills'].append(agent.kills)
            stats['deaths'].append(agent.deaths)
            stats['score'].append(agent.score)
            stats['id'].append(agent.id)

        return stats

    def draw_reference_center(self):
        w = 2
        x = (self.win_width/2) - w/2
        y = (self.win_height/2) - w/2
        self._draw_square(x, y, w, color=(255,10,10))

    def draw_scoreboard(self):
        stats_txt = self.format_scoreboard(self.scoreboard_stats())
        for key, txt in stats_txt.items():
            if key == 'team': continue
            curr_sb = self.scoreboard[key]
            curr_sb.text = txt
            curr_sb.draw()

        # draw team names
        line_height = 18.
        y_offset = line_height * 3
        for i, (index, team) in enumerate(stats_txt['team'].items()):
            y_offset += line_height * (index + 1 + i)
            team_txt = team.name
            team_txt += ' ' * 4
            team_txt += str(team.score)
            txt = self.team_names[team.id]
            txt.set_position(txt.x, txt.yoff - y_offset)
            txt.text = team_txt
            txt.color = list(team.color) + [255]
            txt.draw()

    def set_chat_messages(self):
        blanks = 0
        for i, msg in enumerate(ChatClientGlobal.chatRender.messages()):
            if not msg.payload.content.strip():
                blanks += 1
                continue
            txt = '%s: %s' % (msg.name, msg.payload.content,)
            if msg.payload.channel == 'system':
                color = (40, 255, 0, 255,)
            elif msg.payload.channel == 'pm_%d' % (NetClientGlobal.connection.client_id,):
                color = (120, 200, 200, 255,)
            else:
                color = (255, 40, 0, 255,)
            
            init_c_lib.HUD.set_chat_message(i, txt, color)

        for n in range(i-blanks+1,i):
            init_c_lib.HUD.set_chat_message(n, '', (255,255,255,255,))

        init_c_lib.HUD.set_chat_input_string(ChatClientGlobal.chatRender.user_input())

    def draw(self, fps=None, ping=None, cube_selector=False, zoom=False):


        draw_dead = NetClientGlobal.connection.connected\
                    and GameStateGlobal.agent is not None\
                    and GameStateGlobal.agent.dead
        draw_disconnected = not NetClientGlobal.connection.connected

        draw_fps = fps is not None
        try:
            fps = float(fps)
        except (TypeError, ValueError):
            fps = 0.

        draw_ping = ping is not None
        try:
            ping = int(ping)
        except (TypeError, ValueError):
            ping = 0

        draw_player_stats = True

        draw_chat = True
        draw_chat_input = InputGlobal.input == 'chat'
        draw_chat_cursor = InputGlobal.input == 'chat'

        equipment_slot = -1
        if GameStateGlobal.agent:
            equipment_slot = GameStateGlobal.agent.hud_equipment_slot()
        draw_equipment = True

        draw_compass = True
        draw_map = InputGlobal.map

        init_c_lib.HUD.set_draw_settings(
            zoom,
            cube_selector,
            InputGlobal.inventory,
            InputGlobal.help_menu,
            draw_disconnected,
            draw_dead,
            draw_fps,
            fps,
            draw_ping,
            ping,
            draw_player_stats,
            draw_chat,
            draw_chat_input,
            draw_chat_cursor,
            InputGlobal.scoreboard,
            draw_equipment,
            equipment_slot,
            draw_compass,
            draw_map
        )

        # save the vn code somewhere else. dont delete it yet
        #if InputGlobal.vn:
            #init_c_lib.VN.draw()
            #return
            
        # draw non-text first
        init_c_lib.HUD.draw_textures()
        
        if zoom:
            self.draw_text_items(zoom)
            return
            
        # draw text
        self.draw_text_items(zoom)

    def draw_text_items(self, zoom):
        self.set_chat_messages()

        init_c_lib.CyText.start()

        #if InputGlobal.scoreboard:
            #self.draw_scoreboard()

        init_c_lib.HUD.draw_text()

        init_c_lib.CyText.end()
