#!/c/Python27/python.exe

# boot hack
import sys
import os
print "Working Directory: %s" % (os.getcwd())
sys.path.insert(0, './ext/')
sys.path.insert(0, '/c/dc_mmo/netclient/ext/')
sys.path.insert(0, 'c:/dc_mmo/netclient/ext/')

# load arguments & settings
import args_client
import opts
opts.opts = args_client.get_args()
opts = opts.opts

import math
import time
import random

#import SDL
import stats
import intervals
#import SDL.gl
#import SDL.hud
import vox_lib
import c_lib.c_lib_input as cInput
import sound.sounds as sounds
import world

import c_lib.terrain_map
import init_c_lib
import c_lib.c_lib_objects
import c_lib.c_lib_agents
import c_lib.c_lib_hud as cHUD
import c_lib.c_lib_agents as cAgents

from init_c_lib import StartPhysicsTimer, PhysicsTimerTickCheck
from init_c_lib import START_CLOCK, GET_TICK
from init_c_lib import _pviz_draw
from init_c_lib import NetClientTick, NetClientConnect
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
from hud import Hud
from animations import animations

c_lib.c_lib_sdl.set_resolution(opts.width, opts.height, fullscreen=(int(opts.fullscreen)))
c_lib.c_lib_sdl.init_particles()
c_lib.c_lib_sdl.set_resolution(opts.width, opts.height, fullscreen=opts.fullscreen) # remove this once SDL_functions migrated to c_lib

c_lib.terrain_map.set_view_distance(128) #set view distance for terrain map
c_lib.c_lib_sdl.camera_callback = c_lib.terrain_map.camera_callback
#c_lib.c_lib_sdl.init_particles()

P2 = c_lib.terrain_map.Profiler()

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

        c_lib.c_lib_sdl.SDL_global.init()
        cInput.init()
        #SDL.hud.init()

        init_c_lib.init()
        cHUD.init_hud()

    def init_inputs(self):
        InputGlobal.init_0(self)
        InputGlobal.init_1(self)

    def init_sound(self):
        if not opts.sound:
            return
        soundfiles = os.listdir('./media/sound/wav/')
        sounds.init(enabled=opts.sound, soundfiles=soundfiles, sfxvol=opts.sfx, musicvol=opts.music)

    def __init__(self):
        self.init_sound()

        self.init_globals()
        self.animations = animations
        self.world = world.World()

        self.camera = Camera(x=0, z=50, rot=-1.)
        self.hud = Hud()

        self.intervals = intervals.Intervals()
        def send_agent_pos():
            if GameStateGlobal.agent is not None:
                NetOut.sendMessage.agent_position(GameStateGlobal.agent)

        cAgents.init_draw_agents()

        self.init_inputs()
        print "App init finished"

    def _exit(self):
        GameStateGlobal.exit = True

    def _on_close(self):
        ChatClientGlobal.chatClient.save()

    def connect(self):
        START_CLOCK() #clock must be started before networking stuff
        NetClientGlobal.connect() #starts connection
        a,b,c,d = opts.server.split(".")
        NetClientConnect(int(a),int(b),int(c),int(d), 0)
        #NetClientConnect(127,0,0,1, 0)

    def mainLoop(self):
        global P, Phy
        self.world.add_player(GameStateGlobal.player) #do something about this
        self.world.add_agent(GameStateGlobal.agent)

        self.connect()
        NetOut.mapMessage.request_chunk_list()

        average = []
        fps_text = None
        ping_text = None
        fps = opts.fps
        ping = opts.fps
        ltick, ctick = 0,0

        if ping:
            ping_n = c_lib.c_lib_sdl.get_ticks()

        self.intervals.set()
        _i = 30

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

        _m = 0

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

            agent = GameStateGlobal.agent

            while True: #physics loop
                tc = GET_TICK()
                if tc == 0 or sl_c > 2:
                    break

                sl_c += 1
                _i+=1
                #neutron_fountain()
                if _i % 30 == 0:
                    pass
                    #c_lib.c_lib_objects._generate_circuit_tree(0,0)
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
                    #c_lib.c_lib_objects._create_neutron(_type,1,35.5,35.5,5.5, x,y,z)
                #if True or _i % 15 == 0:
                for _j_ in range(0,1):
                    v = 3
                    x = 32+ 16*random.random()
                    y = 32+ 16*random.random()
                    z = 40

                    vx = v*(random.random() -0.5)
                    vy = v*(random.random() -0.5)
                    vz = -3.5 #v*(random.random() -0.5)
                    #c_lib.c_lib_objects._create_cspray(x,y,z, vx,vy,vz)

                for _j_ in range(0,5):
                    x = 32+ 16*random.random()
                    y = 32+ 16*random.random()
                    z = 40.
                    vx = v*(random.random() -0.5)
                    vy = v*(random.random() -0.5)
                    vz = -1. #v*(random.random() -0.5)
                    #c_lib.c_lib_objects._create_minivox(x,y,z, vx,vy,vz)

                cInput.process_events()
                cInput.get_key_state()
                if GameStateGlobal.agent:
                    GameStateGlobal.agent.set_button_state()

                NetClientGlobal.connection.attempt_recv()
                self.animations.tick()

                #check if another physics tick is needed
                self.world.tick()
                c_lib.c_lib_objects.tick() ## TESTING

            if sl_c > 2:
                print "Physics: %i ticks this frame" % (sl_c)
            if sl_c > 0:
                _o = -1
                _a_id= 0
                if _o==0:
                    if _m < 50:
                        set_agent_control_state(1,0,0,0, 0,0, 0,0) #f,b,l,r,j,jet, theta,phi
                    elif _m < 100:
                        set_agent_control_state(0,0,0,1, 0,0, 0,0) #f,b,l,r,j,jet, theta,phi
                    elif _m < 150:
                        set_agent_control_state(0,1,0,0, 0,0, 0,0) #f,b,l,r,j,jet, theta,phi
                    elif _m < 200:
                        set_agent_control_state(0,0,1,0, 0,0, 0,0) #f,b,l,r,j,jet, theta,phi

                    if _m == 199:
                        _m = 0
                    else:
                        _m += 1
                if _o==1:
                    if _m < 32:
                        set_agent_control_state(1,0,0,0, 0,0, 0,0) #f,b,l,r,j,jet, theta,phi
                    elif _m < 64:
                        set_agent_control_state(0,0,1,0, 0,0, 0,0) #f,b,l,r,j,jet, theta,phi
                        
                    if _m == 64:
                        _m = 0
                    else:
                        _m += 1
                if _o==2:
                    _r = random.random()
                    #print str(_r)
                    if _r < 0.25:
                        set_agent_control_state(1,0,0,0, 0,0, 0,0) #f,b,l,r,j,jet, theta,phi
                    elif _r < 0.50:
                        set_agent_control_state(0,1,0,0, 0,0, 0,0) #f,b,l,r,j,jet, theta,phi
                    elif _r < 0.75:
                        set_agent_control_state(0,0,1,0, 0,0, 0,0) #f,b,l,r,j,jet, theta,phi
                    else:
                        set_agent_control_state(0,0,0,1, 0,0, 0,0) #f,b,l,r,j,jet, theta,phi
                if _o==3:
                    set_agent_control_state(0,0,0,0, 0,0, 0,0) #f,b,l,r,j,jet, theta,phi
                if _o==4:
                    if _m < 32:
                        set_agent_control_state(1,0,0,0, 0,0, 0,0) #f,b,l,r,j,jet, theta,phi
                    elif _m < 64:
                        set_agent_control_state(0,1,0,0, 0,0, 0,0) #f,b,l,r,j,jet, theta,phi
                        
                    if _m == 64:
                        _m = 0
                    else:
                        _m += 1
                NetClientTick()

            P.event("MapControllerGlobal.mapController.tick()")
            MapControllerGlobal.mapController.tick()
            P.event("Camera Setup")
            if InputGlobal.camera == 'agent':
                self.camera.agent_view(GameStateGlobal.agent)
                first_person = True
            elif InputGlobal.camera == 'camera':
                self.camera.camera_view()
                first_person = False

            self.camera.worldProjection()

            P.event("Draw Terrain")
            c_lib.terrain_map.draw_terrain()
            
            P.event("Draw World")
            #import pdb; pdb.set_trace()
            self.camera.update(poll_mouse = (InputGlobal.input == 'camera'))
            self.world.draw(first_person)
            if GameStateGlobal.agent is not None:
                GameStateGlobal.agent.draw_aiming_direction()
            P.event("Animations Draw")
            self.animations.draw()
            P.event("c_lib_objects.draw()")
            c_lib.c_lib_objects.draw() ## TESTING
            P.event("terrain_map.update_chunks")
            c_lib.terrain_map.update_chunks()
            #camera prospective
            P.event("draw hud")
            if opts.hud:
                self.camera.hudProjection()
                draw_bs = False
                if GameStateGlobal.agent:
                    draw_bs = (GameStateGlobal.agent.weapons.active().type == 3)
                self.hud.draw(fps=fps_text, ping=ping_text, block_selector=draw_bs)

                if opts.diagnostic_hud:
                    c_lib.terrain_map.draw_vbo_indicator(opts.map_vbo_indicator_x_offset, opts.map_vbo_indicator_y_offset, -0.3)
                    P2.draw_perf_graph(opts.fps_perf_graph_x_offset, opts.fps_perf_graph_y_offset,-0.30)
                    _pviz_draw(opts.network_latency_graph_x_offset, opts.network_latency_graph_y_offset, -.30)
                    #cHUD.draw_noise_viz(200.0, 200.0, -0.5) #noise histogram
                cHUD._draw_inventory(opts.inventory_hud_x_offset, opts.inventory_hud_y_offset)

            P.event("SDL flip")
            c_lib.c_lib_sdl.SDL_global.flip()
            P.event("Misc")
            #FPS calculation
            if fps:
                ctick = c_lib.c_lib_sdl.get_ticks()
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
                if c_lib.c_lib_sdl.get_ticks() - ping_n > opts.ping_update_interval:
                    # do ping stuff here
                    ping_n = c_lib.c_lib_sdl.get_ticks()
                    NetOut.miscMessage.ping()
                    ping_text = stats.last_ping

            self.intervals.process()

            agent = GameStateGlobal.agent
            if opts.sound:
                if agent:
                    sounds.update(agent.listener_state())
                else:
                    sounds.update()

            P.finish_frame()
        if opts.sound:
            sounds.done()


if __name__ == '__main__':
    app = App()
    app.mainLoop()
else:
    print "Not Run as Main!"
#app.mainLoop()
