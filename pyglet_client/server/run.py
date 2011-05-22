from time import sleep

#import cython
import pyximport #; pyximport.install()

#from pudb import set_trace; set_trace()

from net_server import NetServer
from net_out import NetOut
from net_event import NetEvent
from game_state import GameStateGlobal
from chat_server import ChatServer

import random
def load_map():
    m = GameStateGlobal.terrainMap
    print "Start map generation"
    x_max = 64
    y_max = 64
    z_max = 16
    for xa in range(0, x_max):
        for ya in range(0, y_max):
            for za in range(0, z_max):
                rnd = random.randint(0,64)
                if rnd < 16:
                    rnd2 = random.randint(1,4)
                    m.set(xa,ya,za, rnd2)
    print "Finished map generation"

class Main:

    def __init__(self):
        #setup
        NetServer().init()
        NetOut().init()
        NetEvent().init()
        GameStateGlobal().init()
        ChatServer().init()

    def run(self):
        print "Server Started"
        load_map()
        print "Create Agent"
        GameStateGlobal.agentList.create(-10, 0, -3, 0, 0)

        while True:
            NetServer.serverListener.accept() #accept incoming connections
            NetServer.connectionPool.process_events() #check for new data
            GameStateGlobal.gameState.tick()
            NetOut.event.process_events()
            sleep(GameStateGlobal.TICK)

if __name__ == "__main__":
    print "starting server"
    main = Main()
    main.run()
