import opts
opts = opts.opts

import init_c_lib

from net_client import NetClientGlobal

'''
HUD overlay
'''

class Hud(object):

    def __init__(self):
        init_c_lib.Font.init()

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
        init_c_lib.HUD.draw()
