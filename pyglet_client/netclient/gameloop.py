
if False:
    print "test"
    import sys
    sys.path += ['./imports/lib/python2.6/site-packages/', './']
    sys.path += ['python26.zip']
    print "Path="
    print sys.path
    print ""
    import site

import math

import args_client
import opts
opts.opts = args_client.get_args()

import default_settings as settings
import stats

import intervals

if True:
    import SDL.gl
    import SDL.input
    import SDL.hud
    import vox_lib

    SDL.gl.set_resolution(opts.opts.width, opts.opts.height, fullscreen=int(opts.opts.fullscreen))

    import cube_lib.terrain_map
    cube_lib.terrain_map.set_view_distance(128) #set view distance for terrain map
    SDL.gl.camera_callback = cube_lib.terrain_map.camera_callback
    #SDL.gl.init_particles()
    import init_c_lib
    import c_lib.c_lib_objects

    P2 = cube_lib.terrain_map.Profiler()

##profiler
from profiler import P

from net_client import NetClientGlobal
from net_out import NetOut
from net_event import NetEventGlobal

from game_state import GameStateGlobal
from client_event import ClientEventGlobal
from input import InputGlobal
from chat_client import ChatClientGlobal

from map_controller import MapControllerGlobal

from players import Player
from input import Mouse, Keyboard
from camera import Camera
from hud_sdl import Hud

import world #deprecate

from animations import animations

import random #remove?

#import hotshot
import time


#physics timer
class Physics_loop_timer:

    def __init__(self):
        self.last = 0
        self.delta = 0
        self.tick = 0
        self.TPS = 30 #ticks per second

    def sync(self):
        ct = SDL.gl.get_ticks()
        self.delta += ct - self.last

        #print "ct= %i, last= %i, delta= %i, acm_delta= %i" % (ct, self.last, ct - self.last, self.delta)
        self.last = ct
        if self.delta > 33:
            self.delta -= 33
            self.tick += 1
            return True
        else:
            return False

Phy = Physics_loop_timer()

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
        GameStateGlobal.init_1()
        NetEventGlobal.init_1()
        NetOut.init_1()
        ChatClientGlobal.init_1()
        #MapChunkManagerGlobal.init_1()
        MapControllerGlobal.init_1()

        self.SDL_global = SDL.gl.SDL_global #drawing stuff
        self.SDL_global.init()
        SDL.input.init()
        SDL.hud.init()
        #cube_lib.terrain_map.init()

        init_c_lib.init()

    def init_inputs(self):
        InputGlobal.init_0(self)
        InputGlobal.init_1(self)

    def __init__(self):
        #return
        self.init_globals()
        self.animations = animations
        #other
        self.world = world.World()  #deprecate?

        self.camera = Camera(None, x=0, z=50, rot=-1.)
        self.hud = Hud()

        self.intervals = intervals.Intervals()
        def send_agent_pos():
            if GameStateGlobal.agent is not None:
                NetOut.sendMessage.agent_position(GameStateGlobal.agent)
        self.intervals.register(send_agent_pos, 500)

        self.init_inputs()
        print "App init finished"

    def _exit(self):
        GameStateGlobal.exit = True

    def _on_close(self):
        ChatClientGlobal.chatClient.save()

    def connect(self):
        NetClientGlobal.connect() #starts connection

    def mainLoop(self):
        global P, Phy
        self.world.add_player(GameStateGlobal.player) #do something about this
        self.world.add_agent(GameStateGlobal.agent)

        self.connect()
        #ChatClientGlobal.on_connect()
        NetOut.mapMessage.request_chunk_list()

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

        v2 = vox_lib.Vox_loader().load('html_ed_test.vox')
        #v3 = vox_lib.Vox_loader().load('base.vox')
        #v3.color('blue', base_color='black')
        #v3.move(10,10,15, 0)

        if ping:
            ping_n = SDL.gl.get_ticks()

        self.intervals.set()
        _i = 30
        while not GameStateGlobal.exit:

            P2.start_frame() #TEST
            theta += -.005 #test
            P.start_frame()
            tc = 0
            P.event("Physics Tick")
            while Phy.sync():
                _i+=1
                if _i % 350 == 0:
                    c_lib.c_lib_objects._create_grenade(5,5,2, 0, 0, 50)
                if False or _i % 15 == 0:
                    v = 4
                    x = v*random.random() -0.5
                    y = v*random.random() -0.5
                    z = v*random.random() -0.5
                    le = math.sqrt(x**2+y**2+z**2)
                    x *= v / le
                    y *= v / le
                    z *= v / le
                    c_lib.c_lib_objects._create_grenade(25,25,-4, x,y,z)
                if _i % 150 == 0:
                    v = 2
                    x = v*(random.random() -0.5)
                    y = v*(random.random() -0.5)
                    z = v*(random.random() -0.5)
                    le = math.sqrt(x**2+y**2+z**2)
                    x *= v / le
                    y *= v / le
                    z *= v / le
                    #_type = random.randint(0,9*3)
                    _type=0
                    c_lib.c_lib_objects._create_neutron(_type,1,35.5,35.5,5.5, x,y,z)
                tc += 1
                P.event("process_events")
                SDL.input.process_events()
                P.event("get_key_state")
                SDL.input.get_key_state()
                #network events
                #if GameStateGlobal.agent is not None:
                    #NetOut.sendMessage.send_agent_control_state(GameStateGlobal.agent)
                if GameStateGlobal.agent is not None:
                    NetOut.sendMessage.agent_angle(GameStateGlobal.agent)
                P.event("process incoming packets")
                NetClientGlobal.connection.attempt_recv()
                #check if another physics tick is needed
                self.world.tick()
                self.animations.tick()
                c_lib.c_lib_objects.tick() ## TESTING
            if tc > 1:
                pass
            P.event("MapControllerGlobal.mapController.tick()")
            MapControllerGlobal.mapController.tick()
            if InputGlobal.camera == 'agent':
                self.camera.agent_view(GameStateGlobal.agent)
                first_person = True
            elif InputGlobal.camera == 'camera':
                self.camera.camera_view()
                first_person = False

            self.camera.worldProjection()
            P.event("animations.draw")

            v2.move(10,0,10,theta)
            v2.draw()

            P.event("Draw Terrain")
            cube_lib.terrain_map.draw_terrain()
            self.world.draw(first_person)
            self.animations.draw()
            c_lib.c_lib_objects.draw() ## TESTING
            P.event("Draw World_0")
            P.event("Draw World_1 - laser")
            if True:
                pass
                density = 5
                width = 1.0
                SDL.gl.planar_laser2(density, width, 0,0,8, 0,0,3)
                #SDL.gl.planar_laser(2,2,1, 2,2,6)
                #SDL.gl.planar_laser(4,4,1, 4,4,6)

            if False:
                SDL.gl.draw_particle(0, 1, 5,5,5)
                SDL.gl.draw_particle(1, 2, 5,5,7)
                SDL.gl.draw_particle(2, 2, 5,5,9)
                SDL.gl.draw_particle(3, 2, 5,5,11)
                SDL.gl.draw_particle(4, 2, 5,5,13)
                SDL.gl.draw_particle(5, 2, 5,5,15)
            P.event("terrain_map.update_chunks")
            cube_lib.terrain_map.update_chunks()
            #cube_lib.VBO.draw_chunks()
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
            P.event("draw hud")
            if draw_hud:
                self.camera.hudProjection()
                draw_bs = False
                if GameStateGlobal.agent:
                    draw_bs = (GameStateGlobal.agent.weapons.active().type == 3)
                self.hud.draw(fps=fps_text, ping=ping_text, block_selector=draw_bs)
                cube_lib.terrain_map.draw_vbo_indicator(50,50, -0.3)
                P2.draw_perf_graph(50,500,-0.30)
            P.event("SDL flip")
            self.SDL_global.flip()
            P.event("Misc")
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
                if SDL.gl.get_ticks() - ping_n > settings.ping_update_interval:
                    # do ping stuff here
                    ping_n = SDL.gl.get_ticks()
                    NetOut.miscMessage.ping()
                    ping_text = stats.last_ping

            self.intervals.process()
            P.finish_frame()
            #import pdb; pdb.set_trace()
        #p.stop()
        #self.win.close()

if __name__ == '__main__':
    app = App()
    app.mainLoop()
else:
    print "Not Run as Main!"
#app.mainLoop()
