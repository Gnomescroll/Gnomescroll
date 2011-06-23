
import settings
### DEPRECATE
if settings.pyglet:
    from pyglet import clock, font, image, window
    from pyglet.gl import *

    from pyglet.window import key
else:
    import SDL.gl
    import SDL.input
    import cube_lib.VBO

#import cython
#import pyximport; pyximport.install()

from net_client import NetClientGlobal
from net_out import NetOut
from net_event import NetEventGlobal

from game_state import GameStateGlobal
from client_event import ClientEventGlobal
from input import InputGlobal
from chat_client import ChatClientGlobal

#from cube_dat import CubeGlobal

#from map_chunk_manager import MapChunkManagerGlobal
from map_controller import MapControllerGlobal


from players import Player
from input import Mouse, Keyboard
from camera import Camera
from hud import Hud

import world #deprecate


import random #remove

#import hotshot

class App(object):

    def init_globals(self):
        #stage 1
        NetClientGlobal.init_0()
        #CubeGlobal.init_0()
        GameStateGlobal.init_0()
        NetEventGlobal.init_0()
        NetOut.init_0()
        ChatClientGlobal.init_0()
        #MapChunkManagerGlobal.init_0()
        MapControllerGlobal.init_0()
        #stage 2
        NetClientGlobal.init_1()
        #CubeGlobal.init_1()
        GameStateGlobal.init_1()
        NetEventGlobal.init_1()
        NetOut.init_1()
        ChatClientGlobal.init_1()
        #MapChunkManagerGlobal.init_1()
        MapControllerGlobal.init_1()
        if settings.pyglet == False:
            self.SDL_global = SDL.gl.SDL_global #drawing stuff
            self.SDL_global.init()
            SDL.input.init()
            ## VBO TEST
            cube_lib.VBO.init()
            cube_lib.VBO.test_chunk()

    def init_inputs(self):
        InputGlobal.init_0(self)
        InputGlobal.init_1(self)

    def __init__(self):
        self.init_globals()
        #other
        self.world = world.World()  #deprecate?

        #deprecate
        if settings.pyglet:
            self.win = window.Window(fullscreen=False, vsync=False)
            self.win.on_close = self._on_close
            self.camera = Camera(self.win)
            self.hud = Hud(self.win)
        else:
            self.camera = Camera(None)
            #self.hud = Hud(None)
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

        if settings.pyglet:
            clock.set_fps_limit(60)
            keyboard = key.KeyStateHandler()
            self.win.push_handlers(keyboard)
        #self.win.push_handlers(pyglet.window.event.WindowEventLogger())

        self.connect()
        #ChatClientGlobal.on_connect()
        NetOut.mapMessage.request_chunk_list()

        #p = hotshot.Profile("../log/client.log")
        #p.start()
        average = []
        ltick, ctick = 0,0
        while not self.exit:
            if settings.pyglet:
                self.win.dispatch_events()
                InputGlobal.keyboard.stateHandler(keyboard)
            else:
                SDL.input.process_events()
                SDL.input.get_key_state()
            if GameStateGlobal.agent is not None:
                NetOut.sendMessage.send_agent_control_state(GameStateGlobal.agent)
            #network events
            NetClientGlobal.connection.attempt_recv()
            MapControllerGlobal.mapController.tick() #testing
            self.world.tick()

            if settings.pyglet:
                self.win.clear() #clear window and start drawing

            if InputGlobal.camera == 'agent':
                self.camera.agent_view(GameStateGlobal.agent)
            elif InputGlobal.camera == 'camera':
                self.camera.camera_view()

            self.camera.worldProjection()
            #cube_lib.VBO.draw_test_chunk()
            cube_lib.VBO.update_chunks()
            self.world.draw()
            cube_lib.VBO.draw_chunks()
            #VBO test
            ##cube_lib.VBO.draw_test_chunk()
            #self.SDL_global.set_projection(-1,0,0,0,0)

            if False:
                for i in range(0,256):
                    x = random.random()
                    y = random.random()
                    z = random.random()
                    temp = SDL.gl.draw_line(255,0,0, x,y,z, random.random(),random.random(),random.random())

            if settings.pyglet:
                self.camera.hudProjection()
                self.hud.draw()
                clock.tick()
                self.win.flip()
            else:
                self.SDL_global.flip()
                ctick = SDL.gl.get_ticks()
                #print str(ctick - ltick)
                average.append(ctick-ltick)
                ltick = ctick
                if len(average) > 30:
                    sum = 0.
                    for x in average:
                        sum += float(x)
                    sum = sum / float(len(average))
                    average = []
                    print "mean render time= %f" % (sum)
            #import pdb; pdb.set_trace()
        #p.stop()
        #self.win.close()

if __name__ == '__main__':
    app = App()
    app.mainLoop()
#app.mainLoop()
