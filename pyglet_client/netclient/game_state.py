import math

class GameStateGlobal:

    TICK = 0.01 # seconds
    
    gameState = None
    terrainMap = None
    agentList = None
    playerList = None
    player = None
    agent = None

    def __init__(self):
        pass
        
    @classmethod
    def init_0(self):
        pass
        
    @classmethod
    def init_1(self):
        GameStateGlobal.player = Player()
        GameStateGlobal.agent = PlayerAgent(0, 0, 3, 0, 0, GameStateGlobal.player.id)
        GameStateGlobal.player.agent = GameStateGlobal.agent
        GameStateGlobal.terrainMap = TerrainMap()
        GameStateGlobal.gameState = GameState()
        GameStateGlobal.agentList = AgentList()
        GameStateGlobal.playerList = PlayerList()

    @classmethod
    def update_info(self, player):
        self.player.update_info(player)
        agent = player.get('agent', None)
        if agent is not None: # agent as a property of player is currently optional for server to send
            self.agent.update_info(agent)
        
# main game state wrapper
class GameState:
        
    def __init__(self):
        self.time = 0

    # tick all agents
    #def tick(self):
        #for agent in GameStateGlobal.agentList.values():
            #agent.tick()
        #self.time += 1
        #if self.time % 100 == 0:
            #print "time= %i" % (self.time,)

from terrain_map import TerrainMap
from object_lists import AgentList
from agents import Agent, PlayerAgent
from object_lists import PlayerList
from players import Player
