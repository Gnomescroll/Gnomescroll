

from pyglet import clock, font, image, window
from pyglet.gl import *

#import cython
#import pyximport; pyximport.install()

from player import Player
from input import Mouse, Keyboard
from camera import Camera
from hud import Hud
from pyglet.window import key

#deprecate
import world #deprecate

from net_client import NetClientGlobal
from net_out import NetOut
from net_event import NetEventGlobal
from cube_dat import CubeGlobal
from world_state import WorldStateGlobal
from client_event import ClientEventGlobal

from chat_client import ChatClientGlobal

#import hotshot

class App(object):

    def init_globals(self):
        #stage 1
        NetClientGlobal.init_0()
        CubeGlobal.init_0()
        WorldStateGlobal.init_0()
        NetEventGlobal.init_0()
        NetOut.init_0()
        ChatClientGlobal.init_0()
        #stage 2
        NetClientGlobal.init_1()
        CubeGlobal.init_1()
        WorldStateGlobal.init_1()
        NetEventGlobal.init_1()
        NetOut.init_1()
        ChatClientGlobal.init_1()

    def __init__(self):
        self.init_globals()

        #other
        self.world = world.World()  #deprecate?
        self.win = window.Window(fullscreen=False, vsync=False)
        self.win.on_close = self._on_close
        self.camera = Camera(self.win)
        self.keyboard = Keyboard(self) #move to inputs global
        self.mouse = Mouse(self)       #move to inputs global
        self.hud = Hud(self.win)
        #setup events
        self.keyboard.bind_key_handlers(key.ESCAPE, self.exit)
        self.exit = False
        print "App init finished"

    def exit(self):
        self.exit = True

    def _on_close(self):
        ChatClientGlobal.chatClient.save()

    def mainLoop(self):
        self.world.test_chunk()
        self.world.add_player(WorldStateGlobal.player) #do something about this
        clock.set_fps_limit(60)
        keyboard = key.KeyStateHandler()
        self.win.push_handlers(keyboard)
        #self.win.push_handlers(pyglet.window.event.WindowEventLogger())

        NetClientGlobal.connect() #starts connection
        NetOut.sendMessage.request_chunk_list()

        #p = hotshot.Profile("../log/client.log")
        #p.start()
        self.player = WorldStateGlobal.player
        while not self.exit:
            self.win.dispatch_events()
            #self.keyboard.stateHandler(keyboard)
            [d_x, d_y, d_xa, d_za, jetpack, brake] = self.player.control_state
            NetOut.sendMessage.send_agent_control_state(self.player.id, d_x, d_y, d_xa, d_za, jetpack, brake)
            #network events
            NetClientGlobal.connection.attempt_recv()
            self.world.tick()
            self.win.clear() #clear window and start drawing
            self.camera.worldProjection()
            self.world.draw()
            self.camera.hudProjection()
            self.hud.draw()
            clock.tick()
            self.win.flip()
        #p.stop()
        self.win.close

app = App()
app.mainLoop()
