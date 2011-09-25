#!/usr/bin/python

import sys
sys.path.insert(0, './ext/')

import args_server
import opts
opts.opts = args_server.get_args()

from time import sleep

#import cython
import pyximport #; pyximport.install()

#from pudb import set_trace; set_trace()

import init_c_lib
from init_c_lib import StartPhysicsTimer, PhysicsTimerTickCheck
from init_c_lib import NetServerInit, NetServerTick
from init_c_lib import START_CLOCK, GET_TICK

from net_server import NetServer
from net_out import NetOut
from net_event import NetEvent
from game_state import GameStateGlobal
from chat_server import ChatServer

import c_lib.terrain_map as terrain_map
#from cube_dat import CubeGlobal

import intervals
import file_monitor
import dats.loader as dat_loader

import c_lib.c_lib_objects

import map_gen
import random

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

class Main:

    def __init__(self):
        ##setup
        if opts.opts.map:
            terrain_map.load_from_disk(opts.opts.map)
        else:
            #map_gen.load_map2(terrain_map)
            #map_gen.load_map3(terrain_map)
            #map_gen.grass(terrain_map)
            #map_gen.cave2(terrain_map)
            #map_gen.ore1(terrain_map)

            #map_gen.castle_wall1(terrain_map, 15,15,0, dx=1,dy=1,dz=0, length=13, height=20)
            #map_gen.castle_wall1(terrain_map, _x, _y, dx,dy,dz, length, height):
            map_gen.ground_plane(terrain_map, xsize=64, ysize=64)
            map_gen.castle_tower1(terrain_map, 15,15,1, height=10, wall_length=10)
            #map_gen.castle_tower2(terrain_map, 15,15,1, height=10, wall_length=3)
            if opts.opts.save_map:
                print 'Saving map'
                terrain_map.save_to_disk(opts.opts.save_map)

        #map_gen.ore1(terrain_map)

        #draw_sphere(block=3, radius=30, x=40,y=40,z=90)
        clear_pillar(x=1,y=0)
        clear_pillar(x=0,y=1)
        clear_pillar(x=1,y=1)
        pallet_pillar(0,0,16)

        NetServer.init_0()
        NetOut.init_0()
        NetEvent.init_0()
        GameStateGlobal().init() #conventions
        ChatServer().init() #conventions
        #phase 2 inits
        NetServer.init_1()
        NetOut.init_1()
        NetEvent.init_1()
        #CubeGlobal.init_1()

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
        #import pdb; pdb.set_trace()
        print "Server Started"
        #physics_timer.start_physics_timer(33)
        init_c_lib.init()
        tick = 0
        self.intervals.set()#ms per tick
        #StartPhysicsTimer(33)
        START_CLOCK()
        NetServerInit()
        while True:
            NetServer.serverListener.accept() #accept incoming connections
            NetServer.connectionPool.process_events() #check for new data
            sl_c =0
            while True: #physics loop
                tc = GET_TICK()
                #print "tc=%i" % (tc)
                #tc = PhysicsTimerTickCheck() #get number of ticks server is behind
                if tc == 0 or sl_c > 3: #net out
                    break
                GameStateGlobal.gameState.tick()
                c_lib.c_lib_objects.tick()
                sl_c+=1
                tick+=1
            if sl_c != 0:
                NetServerTick()
            if sl_c > 2:
                print "Physics: %i ticks this frame" % (sl_c)
            NetOut.event.process_events()
            self.intervals.process()
            sleep(0.001)
            #sleep(0.100)
if __name__ == "__main__":
    print "starting server"
    main = Main()
    main.run2()
