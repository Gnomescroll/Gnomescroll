from pyglet import clock, font, image, window
from pyglet.gl import *

#import cython
#import pyximport; pyximport.install()

from pyglet.window import key


from net_client import NetClientGlobal
from net_out import NetOut
from net_event import NetEventGlobal

from game_state import GameStateGlobal
from client_event import ClientEventGlobal
from input import InputGlobal
from chat_client import ChatClientGlobal

from cube_dat import CubeGlobal
from map_chunk_manager import MapChunkManagerGlobal
from map_controller import MapControllerGlobal


from player import Player
from input import Mouse, Keyboard
from camera import Camera
from hud import Hud


import settings  ## put this somewhere!!! so it only has to be in one place

import world #deprecate


import hotshot

class App(object):

    def init_globals(self):
        #stage 1
        NetClientGlobal.init_0()
        CubeGlobal.init_0()
        GameStateGlobal.init_0()
        NetEventGlobal.init_0()
        NetOut.init_0()
        ChatClientGlobal.init_0()
        MapChunkManagerGlobal.init_0()
        MapControllerGlobal.init_0()
        #stage 2
        NetClientGlobal.init_1()
        CubeGlobal.init_1()
        GameStateGlobal.init_1()
        NetEventGlobal.init_1()
        NetOut.init_1()
        ChatClientGlobal.init_1()
        MapChunkManagerGlobal.init_1()
        MapControllerGlobal.init_1()

    def init_inputs(self):
        InputGlobal.init_0(self)
        InputGlobal.init_1(self)

    def __init__(self):
        self.init_globals()
        #other
        self.world = world.World()  #deprecate?
        self.win = window.Window(fullscreen=False, vsync=False)
        self.win.on_close = self._on_close
        self.camera = Camera(self.win)
        self.hud = Hud(self.win)
        #setup events
        self.exit = False

        self.init_inputs()
        print "App init finished"

    def _exit(self):
        self.exit = True

    def _on_close(self):
        ChatClientGlobal.chatClient.save()

    def connect(self):
        NetClientGlobal.connect() #starts connection

    def mainLoop(self):
        #self.world.test_chunk()
        self.world.add_player(GameStateGlobal.player) #do something about this
        self.world.add_agent(GameStateGlobal.agent)
        clock.set_fps_limit(60)
        keyboard = key.KeyStateHandler()
        self.win.push_handlers(keyboard)
        #self.win.push_handlers(pyglet.window.event.WindowEventLogger())

        self.connect()
        #ChatClientGlobal.on_connect()
        NetOut.mapMessage.request_chunk_list()

        self.player = GameStateGlobal.player
        self.agent = GameStateGlobal.agent
        p = hotshot.Profile("../log/client.log")
        p.start()
        while not self.exit:
            self.win.dispatch_events()
            InputGlobal.keyboard.stateHandler(keyboard)
            NetOut.sendMessage.send_agent_control_state(GameStateGlobal.agent)
            #network events
            NetClientGlobal.connection.attempt_recv()
            MapControllerGlobal.mapController.tick() #testing
            self.world.tick()
            self.win.clear() #clear window and start drawing
            if InputGlobal.camera == 'agent':
                self.camera.agent_view(GameStateGlobal.agent)
            elif InputGlobal.camera == 'camera':
                self.camera.camera_view()
            self.camera.worldProjection()
            self.world.draw()
            self.camera.hudProjection()
            self.hud.draw()
            clock.tick()
            self.win.flip()
        p.stop()
        self.win.close()

if __name__ == '__main__':
    app = App()
    app.mainLoop()
#app.mainLoop()
