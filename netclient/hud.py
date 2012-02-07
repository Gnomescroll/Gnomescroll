import opts
opts = opts.opts

import init_c_lib

from chat_client import ChatClientGlobal
from net_client import NetClientGlobal

'''
HUD overlay
'''

class Hud(object):

    def __init__(self):
        init_c_lib.Font.init()

    def set_chat_messages(self):
        if ChatClientGlobal.chatRender is None: return

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

    def draw(self, fps=None, ping=None):

        connected = NetClientGlobal.connection.connected

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

        init_c_lib.HUD.set_draw_settings(
            connected,
            draw_fps,
            fps,
            draw_ping,
            ping,
        )
        init_c_lib.HUD.update_hud_draw_settings()
        
        self.set_chat_messages()
        init_c_lib.HUD.draw()
