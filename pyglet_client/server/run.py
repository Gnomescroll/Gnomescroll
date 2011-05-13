
#import cython
#import pyximport; pyximport.install()

#from server_api import *

from game_state import PlayerAgent, AgentList, GameState
from chat_server import Chat

from server_api import ServerGlobal
from game_state import GameState

import time

class Main:

    serverGlobal = ServerGlobal()
    gameStateGlobal = GameStateGlobal()

    def __init__(self):

        self.chat = Chat()
        self.serverGlobal.messageHandler.chat = self.chat

        self.serverGlobal.init()
        self.gameStateGlobal.init()


#        self.connectionPool = ConnectionPool(self, self.messageHandler)
#        self.serverListener = ServerListener(self.connectionPool)
#        self.eventOut = EventOut(self.connectionPool)

        self.eventOut.gameState = self.gameState
        PlayerAgent.eventOut = self.eventOut
        #globals

    def run(self):
        print "Server Started"
        print "Create Agent"
        self.gameState.agentList.create_agent(-10, 0, -3, 0, 0)
        while True:
            self.serverListener.accept() #accept incoming connections
            self.connectionPool.process_events() #check for new data
            self.gameState.tick()
            self.eventOut.process_events()
            time.sleep(.01)

if __name__ == "__main__":
    print "starting server"
    main = Main()
    main.run()
