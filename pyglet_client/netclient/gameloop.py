
import args_client
import opts
opts.opts = args_client.get_args()

import settings

import ChromeEmbedded
ChromeEmbedded.init()
### DEPRECATE
if settings.pyglet:
    from pyglet import clock, font, image, window
    from pyglet.gl import *

    from pyglet.window import key
else:
    import SDL.gl
    import SDL.input
    import SDL.hud
    import vox_lib
    import cube_lib.VBO

    SDL.gl.set_resolution(opts.opts.width, opts.opts.height, fullscreen=int(opts.opts.fullscreen))
    #vox_lib.test()

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
if settings.pyglet:
    from hud import Hud
else:
    from hud_sdl import Hud

import world #deprecate

from animations import animations

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
            SDL.hud.init()
            ## VBO TEST
            cube_lib.VBO.init()
            cube_lib.VBO.test_chunk()

    def init_inputs(self):
        InputGlobal.init_0(self)
        InputGlobal.init_1(self)

    def __init__(self):

        self.init_globals()
        self.animations = animations
        #other
        self.world = world.World()  #deprecate?

        #deprecate
        if settings.pyglet:
            self.win = window.Window(fullscreen=False, vsync=False)
            self.win.on_close = self._on_close
            self.camera = Camera(self.win)
            self.hud = Hud(self.win)
        else:
            self.camera = Camera(None, x=-100, z=50, rot=-1.)
            self.hud = Hud()
        #setup events
        #self.exit = False

        self.init_inputs()
        print "App init finished"

    def _exit(self):
        GameStateGlobal.exit = True

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
        fps_text = None
        ping_text = None
        fps = opts.opts.fps
        ping = opts.opts.fps
        draw_hud = not opts.opts.no_hud
        ltick, ctick = 0,0

        #TEST
        theta = 0
        v = vox_lib.Vox(0,0,0,0, 8,8,8)
        v.set(5,5,5,255,0,0,0)

        v.set(4,4,0,255,0,255,0)
        v.set(4,4,1,0,255,255,0)
        v.set(4,4,2,0,0,255,0)
        v.set(4,4,3,0,255,255,0)

        v.set(4,4,4,0,255,255,0)
        v.set(4,4,5,0,0,255,0)
        v.set(4,4,6,0,255,255,0)
        v.set(4,4,7,255,0,255,0)

        v.set(0,0,0, 0,255,0,0)
        v.set(0,7,0, 0,255,0,0)
        v.set(7,0,0, 0,255,0,0)
        v.set(7,7,0, 0,255,0,0)

        v.set(0,0,7, 0,255,0,0)
        v.set(0,7,7, 0,255,0,0)
        v.set(7,0,7, 0,255,0,0)
        v.set(7,7,7, 0,255,0,0)

        v2 = vox_lib.Vox_loader().load('agent.vox')
        #v.move(0,0,5, theta)
        #END TEST

        while not GameStateGlobal.exit:
            theta += -.005 #test
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
                first_person = True
            elif InputGlobal.camera == 'camera':
                self.camera.camera_view()
                first_person = False

            self.camera.worldProjection()
            self.animations.draw()

            #vox_lib.draw()
            v.move(0,0,3, theta)
            v.draw()
            v2.move(10,0,10,theta)
            v2.draw()
            #test
            if GameStateGlobal.agent != None:
                a = GameStateGlobal.agent
                x = a.x
                y = a.y
                z = a.z
                x_angle = a.x_angle
                y_angle = a.y_angle
                v.ray_cast2(x,y,z, x_angle, y_angle)
                v2.ray_cast2(x,y,z,x_angle,y_angle)
            #SDL.gl.draw_point(255*random.random(),255*random.random(),255*random.random(), 0.1,0.0,2.0)
            #v.collision_test(0.1,0.0,2.0)
            #cube_lib.VBO.draw_test_chunk()
            cube_lib.VBO.update_chunks()
            self.world.draw(first_person)
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
            #camera prospective
            if draw_hud:
                self.camera.hudProjection()
                self.hud.draw(fps=fps_text, ping=ping_text)
            self.SDL_global.flip()

            #FPS calculation
            if fps:
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
                    #print "mean render time= %f" % (sum)
                    fps_text = "%.2f" % (sum)

            if ping:
                # do ping stuff here
                ping_text = '50'
                pass

            #import pdb; pdb.set_trace()
        #p.stop()
        #self.win.close()

if __name__ == '__main__':
    app = App()
    app.mainLoop()
else:
    print "Not Run as Main!"
#app.mainLoop()
