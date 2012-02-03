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

    def draw_reference_center(self):
        w = 2
        x = (self.win_width/2) - w/2
        y = (self.win_height/2) - w/2
        self._draw_square(x, y, w, color=(255,10,10))

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

        init_c_lib.HUD.draw_textures()
        self.set_chat_messages()
        init_c_lib.HUD.draw_text()
