
#import cython
#import pyximport; pyximport.install()

from server_api import *

from game_state import PlayerAgent, AgentList, GameState
from chat_server import Chat

import time

class Main:

    def __init__(self):
        self.gameState = GameState()
        self.chat = Chat()
        MessageHandler.gameState = self.gameState
        MessageHandler.chat = self.chat
        self.messageHandler = MessageHandler(self)
        DatagramDecoder.messageHandler = self.messageHandler #set global
        self.connectionPool = ConnectionPool(self, self.messageHandler)
        self.serverListener = ServerListener(self.connectionPool)
        self.eventOut = EventOut(self.connectionPool)
        self.eventOut.gameState = self.gameState
        PlayerAgent.eventOut = self.eventOut
        PlayerAgent.gameState = self.gameState
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
