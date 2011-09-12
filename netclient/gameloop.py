
import sys
import os
sys.path.insert(0, './ext/')

import math

import args_client
import opts
opts.opts = args_client.get_args()

#from opts import opts


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
    #import c_lib.c_lib_timer as physics_timer
    import init_c_lib
    from init_c_lib import StartPhysicsTimer, PhysicsTimerTickCheck
    from init_c_lib import START_CLOCK, GET_TICK
    from init_c_lib import _pviz_draw
    P2 = cube_lib.terrain_map.Profiler()
    #from cube_lib.terrain_map import toggle_t_viz_vbo_indicator_style

    from init_c_lib import NetClientTick, NetClientConnect
##profiler
from profiler import P

from net_client import NetClientGlobal
from net_out import NetOut
from net_event import NetEventGlobal

from game_state import GameStateGlobal
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

import sound.sounds as sounds

class App(object):

    def init_globals(self):
        #stage 1
        NetClientGlobal.init_0()
        GameStateGlobal.init_0()
        NetEventGlobal.init_0()
        NetOut.init_0()
        ChatClientGlobal.init_0()
        MapControllerGlobal.init_0()
        #stage 2
        NetClientGlobal.init_1()
        GameStateGlobal.init_1()
        NetEventGlobal.init_1()
        NetOut.init_1()
        ChatClientGlobal.init_1()
        MapControllerGlobal.init_1()

        self.SDL_global = SDL.gl.SDL_global #drawing stuff
        self.SDL_global.init()
        SDL.input.init()
        SDL.hud.init()

        init_c_lib.init()

    def init_inputs(self):
        InputGlobal.init_0(self)
        InputGlobal.init_1(self)

    def init_audio(self):
        audio = opts.opts.audio
        soundfiles = None
        if audio:
            soundfiles = os.listdir('./media/sound/wav/')
        sounds.init(enabled=opts.opts.audio, soundfiles=soundfiles, sfxvol=opts.opts.sfx, musicvol=opts.opts.music)

    def __init__(self):
        self.init_audio()
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
        START_CLOCK() #clock must be started before networking stuff
        NetClientGlobal.connect() #starts connection
        a,b,c,d = opts.opts.server.split(".")
        NetClientConnect(int(a),int(b),int(c),int(d), 0)
        #NetClientConnect(127,0,0,1, 0)

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

        if ping:
            ping_n = SDL.gl.get_ticks()

        c_lib.c_lib_objects._create_agent(0,0,8)

        self.intervals.set()
        _i = 30
        #StartPhysicsTimer(33)
        c_lib.c_lib_objects._generate_circuit_tree(0,0)

        def neutron_fountain():
            v = 2
            x = v*(random.random() -0.5)
            y = v*(random.random() -0.5)
            z = v*(random.random() -0.5)
            le = math.sqrt(x**2+y**2+z**2)
            x *= v / le
            y *= v / le
            z *= v / le
            c_lib.c_lib_objects._create_neutron(0,1,35.5,35.5,5.5, x,y,z)

        print "==="
        print str(opts.opts.server)
        print "==="

        while not GameStateGlobal.exit:
            self.world.sound_updates()

            P2.start_frame() #TEST
            #theta += -.005 #test
            P.start_frame()
            tc = 0
            _density = 1
            _min = 0.025
            _max = 0.9

            P.event("Physics Loop")
            sl_c = 0
            while True: #physics loop
                tc = GET_TICK()
                if tc == 0 or sl_c > 2:
                    break

                sl_c += 1
                _i+=1
                neutron_fountain()
                if _i % 30 == 0:
                    c_lib.c_lib_objects._generate_circuit_tree(0,0)
                if _i % 350 == 0:
                    #c_lib.c_lib_objects._create_grenade(5,5,2, 0, 0, 50, 0, 350)
                    pass
                if False or _i % 15 == 0:
                    v = 4
                    x = v*random.random() -0.5
                    y = v*random.random() -0.5
                    z = v*random.random() -0.5
                    le = math.sqrt(x**2+y**2+z**2)
                    x *= v / le
                    y *= v / le
                    z *= v / le
                    #c_lib.c_lib_objects._create_grenade(25,25,-4, x,y,z, 0, 350)
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
                #if True or _i % 15 == 0:
                for _j_ in range(0,1):
                    v = 3
                    x = 32+ 16*random.random()
                    y = 32+ 16*random.random()
                    z = 40

                    vx = v*(random.random() -0.5)
                    vy = v*(random.random() -0.5)
                    vz = -3.5 #v*(random.random() -0.5)
                    le = math.sqrt(vx**2+vy**2+vz**2)
                    _type=1
                    c_lib.c_lib_objects._create_cspray( _type, x,y,z, vx,vy,vz)
                #P.event("get_key_state")
                #P.event("NetClientTick")
                '''
                SDL.input.get_key_state()
                SDL.input.process_events()
                if GameStateGlobal.agent is not None:
                    NetOut.sendMessage.agent_angle(GameStateGlobal.agent)
                #check if another physics tick is needed
                self.world.tick()
                self.animations.tick()
                c_lib.c_lib_objects.tick() ## TESTING
                NetClientGlobal.connection.attempt_recv()
                '''
                SDL.input.process_events()
                SDL.input.get_key_state()
                if GameStateGlobal.agent is not None:
                    NetOut.sendMessage.agent_angle(GameStateGlobal.agent)
                NetClientGlobal.connection.attempt_recv()
                #check if another physics tick is needed
                self.world.tick()
                self.animations.tick()
                c_lib.c_lib_objects.tick() ## TESTING
            if sl_c > 2:
                print "Physics: %i ticks this frame" % (sl_c)
            if sl_c > 0:
                c_lib.c_lib_objects._set_agent_control_state([0,1,0,0,1,0], 0, 0)
                NetClientTick()

            P.event("MapControllerGlobal.mapController.tick()")
            MapControllerGlobal.mapController.tick()
            if InputGlobal.camera == 'agent':
                self.camera.agent_view(GameStateGlobal.agent)
                first_person = True
            elif InputGlobal.camera == 'camera':
                self.camera.camera_view()
                first_person = False

            self.camera.worldProjection()

            P.event("Draw Terrain")
            #c_lib.c_lib_map_gen._map_density_visualize(1, _min, _max)
            cube_lib.terrain_map.draw_terrain()
            if True:
                c_lib.c_lib_objects._draw_circuit_tree()
            P.event("Draw World")
            #import pdb; pdb.set_trace()
            self.world.draw(first_person) #upto 255 ms
            P.event("Animations Draw")
            self.animations.draw()
            P.event("c_lib_objects.draw()") #up to 5 ms
            c_lib.c_lib_objects.draw() ## TESTING
            P.event("terrain_map.update_chunks")
            cube_lib.terrain_map.update_chunks()
            #camera prospective
            P.event("draw hud")
            if draw_hud:
                self.camera.hudProjection()
                draw_bs = False
                if GameStateGlobal.agent:
                    draw_bs = (GameStateGlobal.agent.weapons.active().type == 3)
                self.hud.draw(fps=fps_text, ping=ping_text, block_selector=draw_bs)
                cube_lib.terrain_map.draw_vbo_indicator(50,50, -0.3)
                P2.draw_perf_graph(50,700,-0.30)
                _pviz_draw(780,400, -.30)

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

            agent = GameStateGlobal.agent
            if agent:
                sounds.update(agent.listener_state())
            else:
                sounds.update()

            P.finish_frame()

        sounds.done()

if __name__ == '__main__':
    app = App()
    app.mainLoop()
else:
    print "Not Run as Main!"
#app.mainLoop()
