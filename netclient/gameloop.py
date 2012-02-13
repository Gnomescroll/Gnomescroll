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

import c_lib.terrain_map
import init_c_lib

from init_c_lib import StartPhysicsTimer, PhysicsTimerTickCheck
from init_c_lib import START_CLOCK, GET_TICK
from init_c_lib import NetClientDispatchNetworkEvents, NetClientConnectTo, NetClientFlushToNet
from profiler import P
from hud import Hud
from dat_loader import dat_loader

init_c_lib.set_resolution(opts.width, opts.height, fullscreen=opts.fullscreen)

class App(object):

    def init_sound(self):
        path = './media/sound/wav/'
        soundfiles = os.listdir(path)
        init_c_lib.Sound.init(path, soundfiles, enabled=opts.sound, sfxvol=opts.sfx, musicvol=opts.music)

    def __init__(self):
        init_c_lib.load_options(opts)
        init_c_lib.init()

        def load_cube_dat():
            import cube_dat
            dat_loader.load('cubes', cube_dat.dat)
        load_cube_dat()

        c_lib.terrain_map.init()
        self.hud = Hud()
        self.init_sound()
        init_c_lib.choose_name(opts.name)
        
        print "Client init finished"

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

    last_tick = 0
    def get_tick(self):
        # get ticks for mouse interpolation
        current_tick = init_c_lib.get_ticks()
        delta_tick = current_tick - self.last_tick
        self.last_tick = current_tick
        return delta_tick

    def mainLoop(self):
        global P, Phy

        self.connect()
        # Server sends the chunk list after client is "ready" (identified & dat loaded)

        average = []
        fps_val = None
        fps = opts.fps
        ping = opts.fps
        ping_n = init_c_lib.get_ticks()
        ltick, ctick = 0,0

        last_tick = 0

        # update mouse
        init_c_lib.update_mouse(self.get_tick())

        while not init_c_lib.cy_input_state.quit:
            P.start_frame()

            tick_count = 0
            _density = 1
            _min = 0.025
            _max = 0.9

            agent = init_c_lib.player_agent_assigned()

            # update mouse
            init_c_lib.update_mouse(self.get_tick())

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

                # update mouse
                init_c_lib.update_mouse(self.get_tick())

                                    
            #this gets triggered if longer than 30ms between render frames
            if phy_ticks >= 2:
                print "Physics: %i ticks this frame" % (phy_ticks)

            # update mouse
            init_c_lib.update_mouse(self.get_tick())

            # updates hud projected display names (doesnt draw them)
            if agent:
                init_c_lib.display_agent_names()

            P.event("Networking 1")
            NetClientFlushToNet()
            NetClientDispatchNetworkEvents() #networking input/output

            # camera input
            P.event("Camera Setup")

            # set input options
            init_c_lib.cy_input_state.set_options(
                opts.invert_mouse,
                opts.sensitivity
            )

            # update mouse
            init_c_lib.update_mouse(self.get_tick())

            # update current camera
            init_c_lib.update_camera_state()

            # world projection
            init_c_lib.camera_world_projection()

            # terrain
            P.event("Draw Terrain")
            c_lib.terrain_map._draw_map()

            # particles
            P.event("Draw voxels and particles")
            init_c_lib.ClientState.draw()

            # animations
            P.event("Draw animations")
            init_c_lib.AnimationDraw()

            # update mouse
            init_c_lib.update_mouse(self.get_tick())

            # hud projection
            P.event("draw hud")
            if opts.hud:
                init_c_lib.camera_hud_projection()
                
                self.hud.draw(fps=fps_val)

                if opts.diagnostic_hud:
                    c_lib.terrain_map.draw_vbo_indicator(opts.map_vbo_indicator_x_offset,opts.map_vbo_indicator_y_offset, -0.3)

            P.event("SDL flip")
            init_c_lib.flip()

            P.event("Misc")

            #FPS calculation
            if fps:
                ctick = init_c_lib.get_ticks()
                average.append(ctick-ltick)
                ltick = ctick
                if len(average) > 30:
                    sum = 0.
                    for x in average:
                        sum += float(x)
                    fps_val = sum / float(len(average))
                    average = []

            if ping:
                #for i in range(300):
                    #init_c_lib.ping()
                _ping_n_now = init_c_lib.get_ticks()
                if _ping_n_now - ping_n > opts.ping_update_interval:
                     ##do ping stuff here
                    ping_n = _ping_n_now
                    for i in range(100):
                        init_c_lib.ping()

            P.finish_frame()

            # update mouse
            init_c_lib.update_mouse(self.get_tick())

            init_c_lib.Sound.update()
            init_c_lib.ClientState.update()

        init_c_lib.close()

if __name__ == '__main__':
    app = App()
    app.mainLoop()
else:
    print "Not Run as Main!"

sys.exit()
