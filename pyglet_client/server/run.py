import args_server
import opts
opts.opts = args_server.get_args()

from time import sleep

#import cython
import pyximport #; pyximport.install()

#from pudb import set_trace; set_trace()

from net_server import NetServer
from net_out import NetOut
from net_event import NetEvent
from game_state import GameStateGlobal
from chat_server import ChatServer

from cube_dat import CubeGlobal

import random
def load_map():
    m = GameStateGlobal.terrainMap
    map_type = 4


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
        x_min = -32
        y_min = -32
        z_min = 0

        x_max = 32
        y_max = 32
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

class Main:

    def __init__(self):
        #setup
        NetServer.init_0()
        NetOut.init_0()
        NetEvent.init_0()
        GameStateGlobal().init() #conventions
        ChatServer().init() #conventions
        CubeGlobal.init_0()
        #phase 2 inits
        NetServer.init_1()
        NetOut.init_1()
        NetEvent.init_1()
        CubeGlobal.init_1()
    def run(self):
        print "Server Started"
        load_map()
        #print "Create Agent"
        #GameStateGlobal.agentList.create(-10, 0, -3, 0, 0)

        while True:
            NetServer.serverListener.accept() #accept incoming connections
            NetServer.connectionPool.process_events() #check for new data
            GameStateGlobal.gameState.tick()
            NetOut.event.process_events()
            sleep(opts.opts.tick)

if __name__ == "__main__":
    print "starting server"
    main = Main()
    main.run()
