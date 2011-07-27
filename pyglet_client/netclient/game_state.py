import math

class GameStateGlobal:

    gameState = None
    #terrainMap = None
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
        cls.projectileList = ProjectileList()
        cls.weaponList = WeaponList()
        #cls.terrainMap = TerrainMap()
        cls.gameState = GameState()
        cls.exit = False
        
    @classmethod
    def init_1(cls):
        pass

    # for your player
    @classmethod
    def update_your_info(cls, player):
        print 'update_your_info'
        if cls.player is None:
            print 'player is None, creating YouPlayer'
            cls.player = cls.playerList.join_yourself(**player)

        print 'update player info, player: %s' % (player,)
        cls.player.update_info(**player)
        agent = player.get('agent', None)
        if agent is not None: # agent as a property of player is currently optional for server to send
            if cls.agent is None:
                cls.agent = GameStateGlobal.agentList.create_player_agent(**agent)
                weapons = agent.get('weapons', None)
                if weapons is not None:
                    cls.agent.weapons.update_info(**weapons)
                cls.player.agent = cls.agent
            else:
                cls.agent.update_info(**agent)
        return cls.player

    @classmethod
    def remove_player(cls, id):
        player = cls.playerList.get(id, None)
        if player is None:
            return
        cls.playerList.destroy(player)

        agent = player.agent
        if agent is not None:
            cls.remove_agent(agent.id, seek=False)

    @classmethod
    def remove_agent(cls, id, seek=True):
        agent = cls.agentList.get(id, None)
        if agent is None:
            return
        cls.agentList.destroy(agent)

        for weapon in agent.weapons:
            cls.remove_weapon(weapon, seek=False)

        if seek:
            owner = cls.playerList.get(agent.owner, None)
            if owner is not None:
                owner.agent = None

    @classmethod
    def remove_weapon(cls, id, seek=True):
        weapon = cls.weaponList.get(id, None)
        if weapon is None:
            return
        cls.weaponList.destroy(weapon)

        if seek:
            owner = cls.agentList.get(weapon.owner, None)
            if owner is not None:
                owner.weapons.drop(weapon)

    @classmethod
    def client_quit(cls, id):
        player = cls.playerList.by_client(id)
        if player is None:
            return
        cls.remove_player(player.id)

    @classmethod
    def scoreboard(cls):
        props = ['name', 'kills', 'deaths', 'score', 'id']
        defs =  [[] for i in range(len(props))]
        stats = dict(zip(props, defs))

        for player in cls.playerList.values():
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

#from terrain_map import TerrainMap
from object_lists import ProjectileList
from object_lists import AgentList
from agents import Agent, PlayerAgent
from object_lists import PlayerList
from object_lists import WeaponList
from players import Player
from net_client import NetClientGlobal

#import cube_lib.terrain_map
import cube_lib.terrain_map as TerrainMap
