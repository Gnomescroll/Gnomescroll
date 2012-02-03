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
import camera

import c_lib.terrain_map
import init_c_lib

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
from dat_loader import dat_loader

#from init_c_lib import _pviz_draw

init_c_lib.set_resolution(opts.width, opts.height, fullscreen=opts.fullscreen)

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
        init_c_lib.Sound.init(path, soundfiles, enabled=opts.sound, sfxvol=opts.sfx, musicvol=opts.music)

    def __init__(self):
        init_c_lib.load_options(opts)

        self.init_globals()

        def load_cube_dat():
            import cube_dat
            dat_loader.load('cubes', cube_dat.dat)
        load_cube_dat()
        
        camera.set_callback(c_lib.terrain_map.camera_callback)
        self.camera = camera.Camera(x=64., y=64., z=128., fov=opts.fov, name='camera')
        self.camera.load()
        self.agent_camera = camera.Camera(x=0., z=50., fov=opts.fov, name='agent', first_person=True)

        self.hud = Hud()

        #gif_maker()
        
        def send_agent_pos():
            if GameStateGlobal.agent is not None:
                NetOut.sendMessage.agent_position(GameStateGlobal.agent)

        self.init_inputs()
        #init_c_lib.load_skybox()

        self.init_sound()
        
        print "App init finished"

    def _exit(self):
        GameStateGlobal.exit = True

    def _on_close(self):
        ChatClientGlobal.chatClient.save()

    def connect(self):
        START_CLOCK() #clock must be started before networking stuff

        try:
            a,b,c,d = map(int, opts.server.split("."))
            print "Ip address: %s" % (opts.server,)
        except ValueError, e:
            print e
            print "Invalid server ip address %s" % (opts.server,)
            sys.exit(1)
        NetClientConnectTo(a,b,c,d, opts.port)

    def mainLoop(self):
        global P, Phy

        self.connect()
        # Server sends the chunk list after client is "ready" (identified & dat loaded)
        #NetOut.mapMessage.request_chunk_list()

        average = []
        fps_val = None
        ping_val = None
        fps = opts.fps
        ping = opts.fps
        ltick, ctick = 0,0

        if ping:
            ping_n = init_c_lib.get_ticks()

        _m = 0

        last_tick = 0

        #init_c_lib.slime_test(30)
            
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
            else:
                GameStateGlobal.agentList.check_for_player_agent()

            P.event("Physics Loop")
            sl_c = 0

            while True: #physics loop
                tc = GET_TICK()
                if tc == 0 or sl_c > 0: #only run once
                    break

                init_c_lib.AnimationTick()
                #if sl_c == 0:
                #    NetClientStartFrame() #physics tick

                sl_c += 1

                #process input
                init_c_lib.process_events()
                init_c_lib.get_key_state()
                if agent:
                    agent.set_button_state()
                    agent.update_sound()
                    agent.display_agent_names()
                    
                init_c_lib.ClientState.tick()

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
            current_tick = init_c_lib.get_ticks()
            delta_tick = current_tick - last_tick
            last_tick = current_tick

            P.event("MapControllerGlobal.mapController.tick()")
            MapControllerGlobal.mapController.tick()
            P.event("Camera Setup")

            if InputGlobal.camera == 'agent':
                self.camera.unload()
                self.agent_camera.load()
                if agent:
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

            #if InputGlobal.hk:
                #init_c_lib.Dragon.draw()

            #P.event("Draw skybox")
            #init_c_lib.render_skybox()

            P.event("Draw Terrain")
            c_lib.terrain_map.draw_terrain()

            #import pdb; pdb.set_trace()

            P.event("Draw voxels and particles")
            init_c_lib.ClientState.draw()

            P.event("Draw animations")
            init_c_lib.AnimationDraw()

            P.event("terrain_map.update_chunks")
            c_lib.terrain_map.update_chunks()
            #camera prospective
            P.event("draw hud")
            if opts.hud:
                camera.camera.hud_projection()
                draw_cube_selector = False
                if agent:
                    draw_cube_selector = (agent.active_weapon == 2)
                self.hud.draw(fps=fps_val, ping=ping_val, cube_selector=draw_cube_selector, zoom=camera.camera.zoomed)

                if opts.diagnostic_hud:
                    c_lib.terrain_map.draw_vbo_indicator(opts.map_vbo_indicator_x_offset,opts.map_vbo_indicator_y_offset, -0.3)
                    P2.draw_perf_graph(opts.fps_perf_graph_x_offset, opts.fps_perf_graph_y_offset,-0.30)
                    #_pviz_draw(opts.network_latency_graph_x_offset,opts.network_latency_graph_y_offset, -.30)

            P.event("SDL flip")
            init_c_lib.flip()
            P.event("Misc")
            #FPS calculation
            if fps:
                ctick = init_c_lib.get_ticks()
                #print str(ctick - ltick)
                average.append(ctick-ltick)
                ltick = ctick
                if len(average) > 30:
                    sum = 0.
                    for x in average:
                        sum += float(x)
                    fps_val = sum / float(len(average))
                    average = []
                    #print "mean render time= %f" % (sum)

            if ping:
                if init_c_lib.get_ticks() - ping_n > opts.ping_update_interval:
                    # do ping stuff here
                    ping_n = init_c_lib.get_ticks()
                    NetOut.miscMessage.ping()
                    ping_val = stats.last_ping

            P.finish_frame()

            init_c_lib.Sound.update()
            init_c_lib.ClientState.update()

        init_c_lib.close()

if __name__ == '__main__':
    app = App()
    app.mainLoop()
else:
    print "Not Run as Main!"
#app.mainLoop()

sys.exit()
