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
        GameStateGlobal.agentList = AgentList()
        GameStateGlobal.playerList = PlayerList()
        player = GameStateGlobal.playerList.join_yourself()
        GameStateGlobal.player = player
        GameStateGlobal.agent = PlayerAgent(player.id, id=0)
        GameStateGlobal.player.agent = GameStateGlobal.agent
        GameStateGlobal.terrainMap = TerrainMap()
        GameStateGlobal.gameState = GameState()

    # for your player
    @classmethod
    def update_info(self, player):
        self.player.update_info(**player)
        agent = player.get('agent', None)
        if agent is not None: # agent as a property of player is currently optional for server to send
            self.agent.update_info(**agent)

    # for other players
    @classmethod
    def load_player_info(self, **player):
        if player['id'] in GameStateGlobal.playerList:
            p = GameStateGlobal.playerList[player['id']]
            p.update_info(**player)
        else:
            p = GameStateGlobal.playerList.join(**player)

    @classmethod
    def load_player_list(self, players):
        for player in players:
            GameStateGlobal.load_player_info(**player)

    @classmethod
    def remove_player(self, id):
        player = GameStateGlobal.playerList.get(id, None)
        if player is None:
            return
        agent = getattr(player, 'agent', None)
        if agent is not None:
            GameStateGlobal.remove_agent(agent.id)
        GameStateGlobal.playerList.leave(player)
        
    @classmethod
    def remove_agent(self, id):
        agent = GameStateGlobal.agentList.get(id, None)
        if agent is None:
            return
        GameStateGlobal.agentList.destroy(agent)
        owner = GameStateGlobal.playerList.get(agent.owner, None)
        if owner is not None:
            owner.agent = None

    @classmethod
    def client_quit(self, id):
        player = GameStateGlobal.playerList.by_client(id)
        if player is None:
            return
        GameStateGlobal.remove_player(player.id)

        
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
from net_client import NetClientGlobal
