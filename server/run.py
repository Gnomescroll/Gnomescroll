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

import random
def load_map():
    m = terrain_map
    map_type = 5


    print "Start map generation"
    if map_type == 1:
        x_min = 0
        y_min = 0
        z_min = 0

        x_max = 64
        y_max = 64
        z_max = 64

        for xa in range(x_min, x_max):
            for ya in range(y_min, y_max):
                for za in range(z_min, z_max):
                    rnd = random.randint(0,64)
                    if rnd < 16:
                        rnd2 = random.randint(1,4)
                        m.set(xa,ya,za, rnd2)
        print "Finished map generation"

    if map_type == 2:
        x_min = -1
        y_min = -1
        z_min = 0

        x_max = 8
        y_max = 8
        z_max = 1

        #plane for testing z collision
        for xa in range(x_min, x_max):
            for ya in range(y_min, y_max):
                for za in range(z_min, z_max):
                    rnd = random.randint(0,64)
                    if True or rnd < 16:
                        rnd2 = random.randint(1,4)
                        rnd2 = 3
                        m.set(xa,ya,za, rnd2)
        #protrusions for testing collision detection
        for xa in range(x_min, x_max):
            for ya in range(y_min, y_max):
                for za in range(z_min, z_max):
                    rnd = random.randint(0,64)
                    if rnd < 16:
                        m.set(xa,ya,za+1, 4)
                        m.set(xa,ya,za+2, 4)
                        m.set(xa,ya,za+3, 5)
                        m.set(xa,ya,za+4, 6)
        print "Finished map generation"

    if map_type == 3:
        x_min = -256
        y_min = -256
        z_min = 0

        x_max = 256
        y_max = 256
        z_max = 1

        #plane for testing z collision
        for xa in range(x_min, x_max):
            for ya in range(y_min, y_max):
                for za in range(z_min, z_max):
                    rnd = random.randint(0,64)
                    if True or rnd < 48:
                        rnd2 = random.randint(1,4)
                        rnd2 = 3
                        m.set(xa,ya,za, rnd2)
        #protrusions for testing collision detection
        for xa in range(x_min, x_max):
            for ya in range(y_min, y_max):
                for za in range(z_min, z_max):
                    rnd = random.randint(0,64)
                    if rnd < 4:
                        m.set(xa,ya,za+1, 4)
                        m.set(xa,ya,za+2, 4)
                        m.set(xa,ya,za+3, 5)
                        m.set(xa,ya,za+4, 6)
        print "Finished map generation"

    if map_type == 4:
        x_min = 0
        y_min = 0
        z_min = 0

        x_max = 128#64
        y_max = 128#64
        z_max = 1

        #plane for testing z collision
        for xa in range(x_min, x_max):
            for ya in range(y_min, y_max):
                for za in range(z_min, z_max):
                    m.set(xa,ya,za, 1)
        #protrusions for testing collision detection
        for xa in range(x_min, x_max):
            for ya in range(y_min, y_max):
                for za in range(z_min, z_max):
                    rnd = random.randint(0,64)
                    if rnd < 8:
                        height =random.randint(1,5)
                        for th in range(1,height):
                            m.set(xa,ya,za+th, 4)
                    elif rnd <10:
                        height =random.randint(1,15)
                        for th in range(1,height):
                            m.set(xa,ya,za+th, 4)
                        #m.set(xa,ya,za+4, 3)
        print "Finished map generation"

    if map_type == 5:
        x_min = 0
        y_min = 0
        z_min = 0

        x_max = 512#64
        y_max = 512#64
        z_max = 1

        #plane for testing z collision
        for xa in range(x_min, x_max):
            for ya in range(y_min, y_max):
                for za in range(z_min, z_max):
                    m.set(xa,ya,za, 2)
        #protrusions for testing collision detection
        for xa in range(x_min, x_max):
            for ya in range(y_min, y_max):
                for za in range(z_min, z_max):
                    rnd = random.randint(0,64)
                    if rnd < 8:
                        height =random.randint(1,5)
                        for th in range(1,height):
                            m.set(xa,ya,za+th, 17)
                    elif rnd <10:
                        height =random.randint(1,15)
                        for th in range(1,height):
                            m.set(xa,ya,za+th, 17)
        print "Finished map generation"

    if map_type == 6:
        m.set(1,1,3, 17)
        m.set(2,2,3, 17)
        #m.set(2,2,3, 17)

def load_map1():
    print 'Begin generating map'
    import noise
    octaves = 6
    f = 200.0 / octaves

    x = 256
    y = 256
    z = 5

    rx = range(x)
    ry = range(y)
    rz = range(z)

    for i in rx:
        for j in ry:
            for k in rz:
                v = noise.pnoise3(i/f, j/f, k/f, octaves=octaves, persistence=10, repeatx=32, base=7)
                if v < 0:
                    p = 0
                else:
                    v *= 100
                    if 0 <= v < 5:
                        p = 1
                    elif 5 <= v < 10:
                        p = 0
                    elif 10 <= v < 15:
                        p = 3
                    elif 15 <= v < 20:
                        p = 0
                    else:
                        p = 5
                terrain_map.set(i,j,k, p)
    print 'Done generating map'

def load_map2():
    print 'start map gen'
    max_height = 15
    from map_gen import Gen
    from random import random
    g = Gen(salt=random())
    h=0
    for i in range(128):
        for j in range(128):
            h = g.getHeight(i,j)
            h = abs(h)
            h *= 100
            h %= max_height
            h = int(h)
            for k in range(h+1):
                terrain_map.set(i, j, k, 2)
            if h==0 or h==1:
                terrain_map.set(i,j, 2, 211)
    print 'done map gen'

class Main:

    def __init__(self):
        #setup
        load_map2()

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
        callbacks = [_dat_change_reload]

        self.file_monitor = file_monitor.FileMonitor(files=files, callbacks=callbacks)
        self.intervals = intervals.Intervals()
        self.intervals.register(self.file_monitor, self.file_monitor.interval)

    def run2(self):
        #import pdb; pdb.set_trace()
        print "Server Started"
        #load_map()
        #load_map2()
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
                sl_c+=1
                tick+=1
            if sl_c != 0:
                NetServerTick()
            if sl_c > 2:
                print "Physics: %i ticks this frame" % (sl_c)
            NetOut.event.process_events()
            self.intervals.process()
            sleep(0.001)

if __name__ == "__main__":
    print "starting server"
    main = Main()
    main.run2()
