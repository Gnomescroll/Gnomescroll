
#import cython
import pyximport #; pyximport.install()

#from server_api import *

#from pudb import set_trace; set_trace()

from chat_server import Chat

from server_api import ServerGlobal
from game_state import GameStateGlobal
from chat_server import ChatServer

import time

class Main:

    serverGlobal = ServerGlobal()
    gameStateGlobal = GameStateGlobal()
    chatServer = ChatServer()

    def __init__(self):
        self.serverGlobal.messageHandler.chat = self.chat
        #setup
        ServerGlobal.init()
        GameStateGlobal.init()
        ChatServer.init()

    def run(self):
        print "Server Started"
        print "Create Agent"
        GameStateGlobal.gameState.agentList.create_agent(-10, 0, -3, 0, 0)

        while True:
            ServerGlobal.serverListener.accept() #accept incoming connections
            ServerGlobal.connectionPool.process_events() #check for new data
            GameStateGlobal.gameState.tick()
            ServerGlobal.eventOut.process_events()
            time.sleep(.01)

if __name__ == "__main__":
    print "starting server"
    main = Main()
    main.run()
