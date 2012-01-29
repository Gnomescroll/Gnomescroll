#!/c/Python27/python.exe

#!/usr/bin/python

# boot hack
import sys
import os
sys.path.insert(0, './ext/')
print "Working Directory: %s" % (os.getcwd())

# set up arguments and settings
import args_server
import opts
opts.opts = args_server.get_args()
opts = opts.opts

import random
import time

import intervals
import map_gen
import file_monitor
import init_c_lib
import c_lib.c_lib_particles
import c_lib.map_gen
import c_lib.terrain_map as terrain_map
import c_lib.c_lib_monsters as cMonsters
import c_lib.c_lib_game_modes as cGameModes
import c_lib.c_lib_options as cOptions
import dats.loader as dat_loader

init_c_lib.init_python_net()
from init_c_lib import StartPhysicsTimer, PhysicsTimerTickCheck
from init_c_lib import NetServerInit,  NetServerDispatchNetworkEvents, NetServerFlushToNet
from init_c_lib import START_CLOCK, GET_TICK
from net_server import NetServer
from net_out import NetOut
from net_event import NetEvent
from game_state import GameStateGlobal
from chat_server import ChatServer

'''
Noise notes:

For each octave:
    amplitude *= persistence
    frequency *= lacunarity

As persistence approaches 1.0, it gets very bumpy. Above 1.0, extremely bumpy.
0.6 seems to be the border for bumpiness

RMF (ridged multifractal) properties:
Always positive
Sharp peaks/ edges.

DO NOT USE SIMPLEX3. probably dont use simplex2 either. it is bad broken code stolen

'''

def _gen_map():
    terrain_map.set_map_dimensions(512,512,128) # TODO:: get this value from the map gen or saved map
    if not opts.map:   # if loading map dont do this debug stuff so angus wont get embarassed
        c_lib.map_gen.init(512,512,128)
        c_lib.map_gen.conf.seed(opts.seed)
        _n = time.time()

        ##base heightmap, smooth shallow hills
        c_lib.map_gen.conf\
        .size(512,512,128)\
        .group(0)\
        .tile(2)\
        .interpolate(4,2,1)\
        .scale(4.0, 4.0, 1.0)\
        .heightmap(baseline=40, maxheight=60)\
        .p2(octaves=6, persistence=0.45)\
        .start()\
        .reset()

         ###base heightmap, smooth shallowER hills
        c_lib.map_gen.conf\
        .size(512,512,128)\
        .group(5)\
        .scale(x=1.0, y=1.0, z=1.0)\
        .tile(2)\
        .interpolate(2,4,1)\
        .heightmap(baseline=30, maxheight=45)\
        .p2(octaves=6, persistence=0.4)\
        .start()\
        .reset()

        c_lib.map_gen.grass(512,512,0)

        ### 3d density noise. floating islands, overhangs
        #c_lib.map_gen.conf\
        #.size(512,512,128)\
        #.tile(2)\
        #.scale(x=3.5, y=3.5, z=1.0)\
        #.gradient2()\
        #.group(1)\
        #.interpolate(4,4,2)\
        #.density(threshold=0.75)\
        #.p3(octaves=6, persistence=0.7)\
        #.start()\
        #.reset()

        ## 3d density noise. floating islands, overhangs, same seed different scale
        c_lib.map_gen.conf\
        .size(512,512,128)\
        .tile(2)\
        .scale(x=4.0, y=4.0, z=1.0)\
        .group(1)\
        .gradient(z0=0.0, z1=-0.3)\
        .interpolate(4,4,2)\
        .density(threshold=0.75)\
        .p3(octaves=6, persistence=0.7)\
        .start()\
        .reset()

        c_lib.map_gen.grass(512,512,0)

        ## 3d density noise, subtractive RMF. forms caves
        c_lib.map_gen.conf\
        .interpolate(4,2,4)\
        .size(512,512,128)\
        .tile(0)\
        .scale(x=4.0, y=4.0, z=2.0)\
        .group(2)\
        .density(threshold=0.97)\
        .gradient2()\
        .rmf()\
        .p3(octaves=6, persistence=0.8)\
        .start()\
        .reset()

         ##3d density noise, subtractive RMF. forms caves
        c_lib.map_gen.conf\
        .interpolate(2,4,2)\
        .size(512,512,128)\
        .gradient2()\
        .tile(0)\
        .scale(x=4.0, y=4.0, z=3.0)\
        .group(3)\
        .density(threshold=0.97)\
        .rmf()\
        .p3(octaves=6, persistence=0.7)\
        .start()\
        .reset()

        """
        New gradient method
        Uses a parabolic falloff
        #"""
        ## 3d density noise. floating islands, overhangs
        #c_lib.map_gen.conf\
        #.size(128,128,128)\
        #.tile(2)\
        #.scale(x=1.0, y=1.0, z=1.0)\
        #.gradient2()\
        #.interpolate(4,4,2)\
        #.density(threshold=0.75)\
        #.p3(octaves=6, persistence=0.7)\
        #.grass()\
        #.start()\
        #.reset()

        #c_lib.map_gen.ceiling(128,128,128,1,3)

        """
        - Perturbing the noisemap tests
        #c_lib.map_gen.conf\
        #.size(128,128,128)\
        #.group(0)\
        #.tile(2)\
        #.interpolate(4,2,1)\
        #.p2(octaves=6, persistence=0.45)\
        #.start()
        #c_lib.map_gen.noise_parameters(octaves=6, persistence=0.9, lacunarity=2.0, frequency=1.0, amplitude=15.0)
        #c_lib.map_gen.perturb2d(128,128, turbulence=1.0, blend='-')
        #c_lib.map_gen.heightmap(128,128,128, 40, 40, base_tile=2)
        #c_lib.map_gen.grass(128,128, 0)
        """

        c_lib.map_gen.destroy()

        print "512,512,128 shit took %0.2f seconds" % (time.time() - _n)


def gen_map_simple():
    size = 128
    terrain_map.set_map_dimensions(size,size,size) # TODO:: get this value from the map gen or saved map

    c_lib.map_gen.init(size,size,size)
    c_lib.map_gen.conf.seed(opts.seed)
    _n = time.time()

    ##base heightmap, smooth shallow hills
    c_lib.map_gen.conf\
    .size(size,size,size)\
    .group(0)\
    .tile(2)\
    .interpolate(4,4,1)\
    .scale(4.0, 4.0, 1.0)\
    .heightmap(baseline=20, maxheight=30)\
    .p2(octaves=6, persistence=0.35,frequency=0.1)\
    .start()\
    .reset()

     ###base heightmap, smooth shallowER hills
    #c_lib.map_gen.conf\
    #.size(128,128,128)\
    #.group(5)\
    #.scale(x=4.0, y=4.0, z=1.0)\
    #.tile(2)\
    #.interpolate(2,4,1)\
    #.heightmap(baseline=30, maxheight=45)\
    #.p2(octaves=6, persistence=0.4)\
    #.start()\
    #.reset()


def pallet_pillar(x,y,z):
    for i in range(0,32):
        m = terrain_map
        m.set(x,y,z+2*i, i)

def clear_pillar(x,y):
     for i in range(0,128):
        m = terrain_map
        m.set(x,y,i, 0)


def draw_sphere(block, radius, x,y,z):
    m = terrain_map
    t = int(radius)+1
    radius = float(radius)
    fx = float(x);fy=float(y);fz=float(z)
    for _x in range(-t,t):
        for _y in range(-t,t):
            for _z in range (-t,t):
                d = (float(_x))**2+(float(_y))**2+(float(_z))**2
                #print "d= %s" % (str(d))
                if d < radius**2:
                    m.set(_x+x,_y+y,_z+z,block)

def good_cave1():
    pass
    c_lib.map_gen.Dragon.generate()
    c_lib.map_gen.Dungeon.generate()
    
    ##base heightmap, smooth shallow hills
    #c_lib.map_gen.conf\
    #.size(128,128,128)\
    #.group(0)\
    #.tile(3)\
    #.interpolate(4,2,1)\
    #.scale(4.0, 4.0, 1.0)\
    #.heightmap(baseline=22, maxheight=6)\
    #.p2(octaves=6, persistence=0.35)\
    #.start()\
    #.reset()
    
    #c_lib.map_gen.conf\
    #.size(128,128,128)\
    #.group(1)\
    #.tile(3)\
    #.interpolate(4,2,1)\
    #.scale(2.0, 2.0, 1.0)\
    #.reverse_heightmap(baseline=37, maxheight=7)\
    #.p2(octaves=6, persistence=0.6)\
    #.start()\
    #.reset()

    #c_lib.map_gen.conf\
    #.size(128,128,128)\
    #.group(0)\
    #.tile(103)\
    #.interpolate(4,2,1)\
    #.scale(4.0, 4.0, 1.0)\
    #.heightmap(baseline=13, maxheight=20, tile=103)\
    #.p2(octaves=6, persistence=0.6)\
    #.start()\
    #.reset()
    
    #c_lib.map_gen.conf\
    #.size(128,128,128)\
    #.group(0)\
    #.tile(0)\
    #.interpolate(4,2,1)\
    #.scale(2.0, 2.0, 1.0)\
    #.reverse_heightmap(baseline=13, maxheight=12, minheight=3, tile=103)\
    #.p2(octaves=6, persistence=0.35)\
    #.start()\
    #.reset()

    #c_lib.map_gen.floor(128,128,14,1,101)


class Main:

    def __init__(self):
        init_c_lib.set_seed(opts.seed)
        cOptions.load(opts)
        
        #gen_map_simple()

        ##setup

        '''
        loading map from file by default because angus gets segfault
        '''
        if None:
            if opts.map and os.path.exists('./content/maps/%s'%opts.map):
                print opts.map
                terrain_map.load_from_disk(opts.map)
                terrain_map.set_map_dimensions(128,128,128)
            else:
                pass
                terrain_map.set_map_dimensions(128,128,128)
                map_gen.floor(terrain_map)
                #terrain_map.load_from_disk("natural_terrain")
        #_gen_map()
        gen_map_simple()
        #terrain_map.load_from_disk("natural2_max")
        #terrain_map.load_from_disk("natural4")
            ##terrain_map.load_from_disk("savetest2")
            ##_gen_map()
            ##terrain_map.load_from_disk("nat5")
            #pass

        #map_gen.grass(terrain_map)
        #map_gen.cave2(terrain_map)
        #map_gen.ore1(terrain_map)

        #map_gen.ground_plane(terrain_map, xsize=256, ysize=256, tile=11)
        
        #map_gen.castle_tower1(terrain_map, 15,15,1, height=10, wall_length=10)
        #map_gen.castle_tower2(terrain_map, 15,5,1, height=10, wall_length=3, tile=10)
        #map_gen.castle_tower2(terrain_map, 30,5,1, height=10, wall_length=5, tile=9)
        #map_gen.castle_tower1(terrain_map, 15,15,1, height=10, wall_length=3, tile=8)
        #map_gen.castle_tower1(terrain_map, 30,15,1, height=10, wall_length=4, tile=10)
        #map_gen.castle_tower1(terrain_map, 45,15,1, height=10, wall_length=5, tile=9)
        #draw_sphere(block=100, radius=30, x=64,y=64,z=90)

        #clear_pillar(x=1,y=0)
        #clear_pillar(x=0,y=1)
        #clear_pillar(x=1,y=1)
        #pallet_pillar(0,0,0)

        #c_lib.map_gen.conf.seed(opts.seed)
        
        #good_cave1()

        if opts.save_map:
            print 'Saving map as %s' % (opts.save_map,)
            terrain_map.save_to_disk(opts.save_map)

        NetServer.init()
        NetOut.init()
        NetEvent.init()
        GameStateGlobal()
        ChatServer()

         #detect dat changes
        def _dat_change_reload(f):
            dat_loader._reload()
            NetOut.event.send_dat()

        files = [
            'dats/projectile.py',
            'dats/item.py',
            'dats/cube.py',
            'dats/weapon.py',
            'dats/cube_dat.py',
        ]
        callbacks = [_dat_change_reload] * len(files)

        self.file_monitor = file_monitor.FileMonitor(files=files, callbacks=callbacks)
        self.intervals = intervals.Intervals()
        self.intervals.register(self.file_monitor, self.file_monitor.interval)

    def run2(self):
        print "Server Started"
        #physics_timer.start_physics_timer(33)
        init_c_lib.init()
        tick = 0
        self.intervals.set()#ms per tick
        #StartPhysicsTimer(33)
        START_CLOCK()

        try:
            a,b,c,d = map(int, opts.ip_address.split('.'))
            print "Ip address: %s" % (opts.ip_address,)
        except ValueError, e:
            print "Invalid ip address %s" % (opts.ip_address,)
            print e
            sys.exit(1)
            
        NetServerInit(a,b,c,d, opts.port)

        #cMonsters.slime_test(30)
        cGameModes.ctf_start()
        while True:
            #NetServer.serverListener.accept() #accept incoming connections
            #NetServer.connectionPool.process_events() #check for new data
            NetServer.connectionPool.dispatch_buffer()
            sl_c =0
            while True: #physics loop
                tc = GET_TICK()
                #print "tc=%i" % (tc)
                #tc = PhysicsTimerTickCheck() #get number of ticks server is behind
                if tc == 0 or sl_c > 3: #net out
                    break
                c_lib.c_lib_particles.tick()
                sl_c+=1
                tick+=1
            #if sl_c != 0:
            #    NetServerDispatchNetworkEvents()
            if sl_c > 1:
                print "Physics: %i ticks this frame" % (sl_c)
            
            NetServerFlushToNet()
            NetServerDispatchNetworkEvents()
            
            NetOut.event.process_events()
            self.intervals.process()

            cMonsters.slime_tick()
            cGameModes.check_agent_proximities()

            time.sleep(0.0001)
            #time.sleep(0.100)
if __name__ == "__main__":
    print "starting server"
    main = Main()
    main.run2()
