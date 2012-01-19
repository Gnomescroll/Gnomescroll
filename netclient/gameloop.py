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

import stats
import intervals
import vox_lib

import camera

import c_lib.terrain_map
import init_c_lib
import c_lib.c_lib_particles as cParticles
import c_lib.c_lib_agents as cAgents
import c_lib.c_lib_hud as cHUD
import c_lib.c_lib_input as cInput
import c_lib.c_lib_sdl as cSDL
import c_lib.c_lib_camera as cCamera
import c_lib.c_lib_sound as cSound
import c_lib.c_lib_options as cOptions
import c_lib.c_lib_map_gen as cMapGen
import c_lib.c_lib_monsters as cMonsters
import c_lib.c_lib_items as cItems

import c_lib.c_lib_animations as cAnimations

init_c_lib.init_python_net()
from init_c_lib import StartPhysicsTimer, PhysicsTimerTickCheck
from init_c_lib import START_CLOCK, GET_TICK
from init_c_lib import NetClientDispatchNetworkEvents, NetClientConnectTo, NetClientFlushToNet
from profiler import P
from net_client import NetClientGlobal
from net_out import NetOut
from net_event import NetEventGlobal
from game_state import GameStateGlobal
from input import InputGlobal
from chat_client import ChatClientGlobal
from map_controller import MapControllerGlobal
from input import Mouse, Keyboard
from hud import Hud
from animations import animations

#from init_c_lib import _pviz_draw

cSDL.set_resolution(opts.width, opts.height, fullscreen=opts.fullscreen)

c_lib.terrain_map.set_view_distance(128) #set view distance for terrain map

P2 = c_lib.terrain_map.Profiler()

class App(object):

    def init_globals(self):
        NetClientGlobal.init()
        GameStateGlobal.init()
        NetEventGlobal.init()
        NetOut.init()
        ChatClientGlobal.init()
        MapControllerGlobal.init()
        init_c_lib.init()

    def init_inputs(self):
        InputGlobal.init(self)

    def init_sound(self):
        path = './media/sound/wav/'
        soundfiles = os.listdir(path)
        cSound.Sound.init(path, soundfiles, enabled=opts.sound, sfxvol=opts.sfx, musicvol=opts.music)

    def __init__(self):
        cOptions.load(opts)

        cMonsters.load_slime_voxel_dat()
        cAgents.load_agent_voxel_dat()
        cItems.load_voxel_dats()

        self.init_globals()
        
        self.animations = animations

        camera.set_callback(c_lib.terrain_map.camera_callback)
        self.camera = camera.Camera(x=64., y=64., z=128., fov=opts.fov, name='camera')
        self.camera.load()
        self.agent_camera = camera.Camera(x=0., z=50., fov=opts.fov, name='agent', first_person=True)

        self.hud = Hud()

        #gif_maker()
        
        self.intervals = intervals.Intervals()
        def send_agent_pos():
            if GameStateGlobal.agent is not None:
                NetOut.sendMessage.agent_position(GameStateGlobal.agent)

        self.init_inputs()
        cCamera.load_skybox()

        self.init_sound()
        
        print "App init finished"

    def _exit(self):
        GameStateGlobal.exit = True

    def _on_close(self):
        ChatClientGlobal.chatClient.save()

    def connect(self):
        START_CLOCK() #clock must be started before networking stuff
        a,b,c,d = opts.server.split(".")
        NetClientConnectTo(int(a),int(b),int(c),int(d), 8080)

    def mainLoop(self):
        global P, Phy

        self.connect()
        # Server sends the chunk list after client is "ready" (identified & dat loaded)
        #NetOut.mapMessage.request_chunk_list()

        average = []
        fps_text = None
        ping_text = None
        fps = opts.fps
        ping = opts.fps
        ltick, ctick = 0,0

        if ping:
            ping_n = cSDL.get_ticks()

        self.intervals.set()
        _i = 30

        cParticles._generate_circuit_tree(0,0)

        _m = 0

        last_tick = 0

        #cMonsters.slime_test()
        while not GameStateGlobal.exit:
            P2.start_frame() #TEST
            #theta += -.005 #test
            P.start_frame()


            NetClientGlobal.connection.dispatch_buffer()

            tc = 0
            _density = 1
            _min = 0.025
            _max = 0.9

            agent = GameStateGlobal.agent
            if agent:
                if agent.camera is None:
                    agent.camera = self.agent_camera

            P.event("Physics Loop")
            sl_c = 0
            while True: #physics loop
                tc = GET_TICK()
                if tc == 0 or sl_c > 0: #only run once
                    break

                cAnimations.AnimationTick()
                #if sl_c == 0:
                #    NetClientStartFrame() #physics tick

                sl_c += 1

                ParticleTestSpawn(_i)
                _i+=1

                #process input
                cInput.process_events()
                cInput.get_key_state()
                if agent:
                    agent.set_button_state()
                #physics tick routine
                self.animations.tick()
                if GameStateGlobal.agent is not None:
                    GameStateGlobal.agent.update_sound()

                cParticles.tick() ## TESTING

            #this gets triggered if longer than 30ms between render frames
            if sl_c >= 2:
                print "Physics: %i ticks this frame" % (sl_c)

            #if there has been at least one physics tick
            if sl_c > 0:
                pass

            '''
                May only want to send output every 30 ms
            '''
            P.event("Networking 1")
            NetClientFlushToNet()
            NetClientDispatchNetworkEvents() #networking input/output
            
            '''
            !?
                What is delta tick used for?
                --mouse motion interpolation
            '''
            #start frame
            current_tick = cSDL.get_ticks()
            delta_tick = current_tick - last_tick
            last_tick = current_tick

            P.event("MapControllerGlobal.mapController.tick()")
            MapControllerGlobal.mapController.tick()
            P.event("Camera Setup")

            if InputGlobal.camera == 'agent':
                self.camera.unload()
                self.agent_camera.load()
                if agent:
                    #agent.update_camera()
                    self.agent_camera.pos(agent.camera_position())
            elif InputGlobal.camera == 'camera':
                self.agent_camera.unload()
                self.camera.load()


            #update camera before drawing
            if InputGlobal.input == 'agent':
                self.agent_camera.input_update(delta_tick)
            elif InputGlobal.input == 'camera':
                self.camera.input_update(delta_tick)

            if agent:
                agent.update_camera()

            camera.camera.world_projection()

            if InputGlobal.hk:
                cMapGen.Dragon.draw()

            #P.event("Draw skybox")
            #cCamera.render_skybox()

            P.event("Draw Terrain")
            c_lib.terrain_map.draw_terrain()

            #import pdb; pdb.set_trace()

            P.event("World.draw(), draw agents")
            
            if opts.draw_agents:
                cAgents.draw_agents()

            P.event("Animations Draw")
            self.animations.draw()

            P.event("c_lib_particles.draw()")
            cParticles.draw() ## TESTING

            cAnimations.AnimationDraw()

            P.event("terrain_map.update_chunks")
            c_lib.terrain_map.update_chunks()
            #camera prospective
            P.event("draw hud")
            if opts.hud:
                camera.camera.hud_projection()
                draw_cube_selector = False
                if agent:
                    draw_cube_selector = (agent.active_weapon == 2)
                self.hud.draw(fps=fps_text, ping=ping_text, cube_selector=draw_cube_selector, zoom=camera.camera.zoomed)

                if opts.diagnostic_hud:
                    c_lib.terrain_map.draw_vbo_indicator(opts.map_vbo_indicator_x_offset,opts.map_vbo_indicator_y_offset, -0.3)
                    P2.draw_perf_graph(opts.fps_perf_graph_x_offset, opts.fps_perf_graph_y_offset,-0.30)
                    #_pviz_draw(opts.network_latency_graph_x_offset,opts.network_latency_graph_y_offset, -.30)

            P.event("SDL flip")
            cSDL.flip()
            P.event("Misc")
            #FPS calculation
            if fps:
                ctick = cSDL.get_ticks()
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

            if False and ping:
                if cSDL.get_ticks() - ping_n > opts.ping_update_interval:
                    # do ping stuff here
                    ping_n = cSDL.get_ticks()
                    NetOut.miscMessage.ping()
                    ping_text = stats.last_ping

            self.intervals.process()

            P.finish_frame()

            cSound.Sound.update()
            cMonsters.slime_tick()

        init_c_lib.close()


def ParticleTestSpawn(_i):
    return
    #neutron_fountain()
    if _i % 30 == 0:
        pass
        #cParticles._generate_circuit_tree(0,0)
    if _i % 350 == 0:
        #cParticles._create_grenade(5,5,2, 0, 0, 50, 0, 350)
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
        #cParticles._create_grenade(25,25,-4, x,y,z, 0, 350)
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
        #cParticles._create_neutron(_type,1,35.5,35.5,5.5, x,y,z)
    #if True or _i % 15 == 0:
    for _j_ in range(0,1):
        v = 3
        x = 32+ 16*random.random()
        y = 32+ 16*random.random()
        z = 40

        vx = v*(random.random() -0.5)
        vy = v*(random.random() -0.5)
        vz = -3.5 #v*(random.random() -0.5)
        #cParticles._create_cspray(x,y,z, vx,vy,vz)

    for _j_ in range(0,5):
        x = 32+ 16*random.random()
        y = 32+ 16*random.random()
        z = 40.
        vx = v*(random.random() -0.5)
        vy = v*(random.random() -0.5)
        vz = -1. #v*(random.random() -0.5)
        #cParticles._create_minivox(x,y,z, vx,vy,vz)


def neutron_fountain():
    v = 2
    x = v*(random.random() -0.5)
    y = v*(random.random() -0.5)
    z = v*(random.random() -0.5)
    le = math.sqrt(x**2+y**2+z**2)
    x *= v / le
    y *= v / le
    z *= v / le
    cParticles._create_neutron(0,1,35.5,35.5,5.5, x,y,z)

def billboard_text_fountain():
    v = 6
    vx = v*(random.random() -0.5)
    vy = v*(random.random() -0.5)
    vz = v*(random.random() -0.5)
    le = math.sqrt(vx**2+vy**2+vz**2)
    vx *= v / le
    vy *= v / le
    vz *= v / le
    vz = 0
    _x = (random.random() -0.5) * 1
    _y = (random.random() -0.5) * 1
    num = random.randint(0,25)
    cParticles._create_billboard_text(16.0+_x,16.0+_y,125.0, vx, vy,vz, str(num))


def gif_maker():
    z_step = 0.0013
    freq = 9
    seed = 9090
    distance_enabled = True
    w = 256
    h = 256
    gradient = 6
    turb_enable = False
    turb_f = 1
    turb_pow = 1
    cHUD.VN.configure(z_step, freq, seed, distance_enabled, turb_enable, turb_f, turb_pow)
    #cHUD.VN.configure(z_step, freq, seed, distance_enabled, turb_f, turb_pow)
    cHUD.VN.init(width=w, height=h, gradient=gradient)
    print 'preparing to generate frames'
    secs = 1./30.
    #secs = 1.
    cHUD.VN.frames(secs)
    print 'frames generated'
    sys.exit()

if __name__ == '__main__':
    app = App()
    app.mainLoop()
else:
    print "Not Run as Main!"
#app.mainLoop()

sys.exit()

'''
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
'''
