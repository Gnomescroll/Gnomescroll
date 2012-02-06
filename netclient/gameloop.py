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

import stats

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
from chat_client import ChatClientGlobal
from map_controller import MapControllerGlobal
from hud import Hud
from dat_loader import dat_loader

#from init_c_lib import _pviz_draw

init_c_lib.set_resolution(opts.width, opts.height, fullscreen=opts.fullscreen)

c_lib.terrain_map.set_view_distance(256) #set view distance for terrain map

P2 = c_lib.terrain_map.Profiler()

class App(object):

    def init_globals(self):
        NetClientGlobal.init()
        NetEventGlobal.init()
        NetOut.init()
        MapControllerGlobal.init()
        init_c_lib.init()

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
        
        init_c_lib.camera_callback = c_lib.terrain_map.camera_callback

        self.hud = Hud()

        self.init_sound()
        
        print "App init finished"

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
        ping_n = init_c_lib.get_ticks()
        ltick, ctick = 0,0

        last_tick = 0

        while not init_c_lib.cy_input_state.quit:
            P2.start_frame()
            P.start_frame()

            NetClientGlobal.connection.dispatch_buffer()

            tick_count = 0
            _density = 1
            _min = 0.025
            _max = 0.9

            agent = init_c_lib.player_agent_assigned()
            if init_c_lib.identified():
                ChatClientGlobal.init()

            P.event("Physics Loop")
            phy_ticks = 0
            while True: 
                tick_count = GET_TICK()
                if tick_count == 0 or phy_ticks > 0: #only run once
                    break
                phy_ticks += 1

                init_c_lib.process_input()
                init_c_lib.AnimationTick()                    
                init_c_lib.ClientState.tick()
                if agent:
                    init_c_lib.update_sound_listener()
                                    
            #this gets triggered if longer than 30ms between render frames
            if phy_ticks >= 2:
                print "Physics: %i ticks this frame" % (phy_ticks)

            # updates hud projected display names (doesnt draw them)
            if agent:
                init_c_lib.display_agent_names()

            # update chat client input buffer
            ChatClientGlobal.load_buffer_from_c()

            P.event("Networking 1")
            NetClientFlushToNet()
            NetClientDispatchNetworkEvents() #networking input/output

            # get ticks for mouse interpolation
            current_tick = init_c_lib.get_ticks()
            delta_tick = current_tick - last_tick
            last_tick = current_tick

            P.event("MapControllerGlobal.mapController.tick()")
            MapControllerGlobal.mapController.tick()

            # camera input
            P.event("Camera Setup")

            # set input options
            init_c_lib.cy_input_state.set_options(
                opts.invert_mouse,
                opts.sensitivity
            )

            # update current camera
            init_c_lib.update_camera_state(delta_tick)

            # world projection
            init_c_lib.camera_world_projection()

            # terrain
            P.event("Draw Terrain")
            c_lib.terrain_map.draw_terrain()

            # particles
            P.event("Draw voxels and particles")
            init_c_lib.ClientState.draw()

            # animations
            P.event("Draw animations")
            init_c_lib.AnimationDraw()

            # update map chunks
            P.event("terrain_map.update_chunks")
            c_lib.terrain_map.update_chunks()

            # hud projection
            P.event("draw hud")
            if opts.hud:
                init_c_lib.camera_hud_projection()
                
                self.hud.draw(fps=fps_val, ping=ping_val)

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

sys.exit()
