import math

class GameStateGlobal:

    TICK = 0.01 # seconds

    gameState = None
    terrainMap = None
    agentList = None
    playerList = None
    projectileList = None

    player = None
    agent = None

    def __init__(self):
        pass

    @classmethod
    def init_0(cls):
        cls.agentList = AgentList()
        cls.playerList = PlayerList()
        player = GameStateGlobal.playerList.join_yourself()
        cls.player = player
        cls.agent = player.agent
        cls.projectileList = ProjectileList()
        cls.player.agent = GameStateGlobal.agent
        cls.terrainMap = TerrainMap()
        cls.gameState = GameState()

    @classmethod
    def init_1(cls):
        pass

    # for your player
    @classmethod
    def update_info(cls, player):
        cls.player.update_info(**player)
        agent = player.get('agent', None)
        if agent is not None: # agent as a property of player is currently optional for server to send
            cls.agent.update_info(**agent)
        return cls.player

    # for other players
    @classmethod
    def load_player_info(cls, **player):
        print 'load_player_info'
        print player
        if 'id' not in player:
            return
        if player['id'] in GameStateGlobal.playerList:
            p = GameStateGlobal.playerList[player['id']]
            p.update_info(**player)
        else:
            p = GameStateGlobal.playerList.join(**player)

    @classmethod
    def load_agent_info(cls, **agent):
        print 'load_agent_info'
        print agent
        if 'id' not in agent:
            return
        aid = agent['id']
        if aid in GameStateGlobal.agentList:
            a = GameStateGlobal.agentList[aid]
            a.update_info(**agent)
        else:
            a = GameStateGlobal.agentList.create(**agent)

    @classmethod
    def load_player_list(cls, players):
        for player in players:
            GameStateGlobal.load_player_info(**player)

    @classmethod
    def load_agent_list(cls, agents):
        for agent in agents:
            GameStateGlobal.load_agent_info(**agent)

    @classmethod
    def remove_player(cls, id):
        print 'gsg remove_player'
        player = GameStateGlobal.playerList.get(id, None)
        if player is None:
            print 'player is none'
            return
        agent = getattr(player, 'agent', None)
        if agent is not None:
            print 'removing agent'
            GameStateGlobal.remove_agent(agent.id)
        print agent
        GameStateGlobal.playerList.leave(player)

    @classmethod
    def remove_agent(cls, id):
        print 'gsg remove_agent'
        agent = GameStateGlobal.agentList.get(id, None)
        if agent is None:
            print 'agent is none'
            return
        GameStateGlobal.agentList.destroy(agent)
        owner = GameStateGlobal.playerList.get(agent.owner, None)
        if owner is not None:
            owner.agent = None

    @classmethod
    def client_quit(cls, id):
        player = GameStateGlobal.playerList.by_client(id)
        if player is None:
            return
        GameStateGlobal.remove_player(player.id)

    @classmethod
    def scoreboard(cls):
        props = ['name', 'kills', 'deaths', 'score', 'id']
        defs =  [[] for i in range(len(props))]
        stats = dict(zip(props, defs))

        for player in GameStateGlobal.playerList.values():
            stats['name'].append(player.name)
            stats['kills'].append(player.kills)
            stats['deaths'].append(player.deaths)
            stats['score'].append(player.score())
            stats['id'].append(player.id)

        return stats

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
from object_lists import ProjectileList
from object_lists import AgentList
from agents import Agent, PlayerAgent
from object_lists import PlayerList
from players import Player
from net_client import NetClientGlobal
