import math

import cube_lib.terrain_map as terrain_map
from opts import opts

class GameStateGlobal:

    gameState = None
    #terrainMap = None
    agentList = None
    playerList = None
    #state
    agent_id = 0
    player_id = 0
    projectile_id = 0

    def __init__(self):
        GameStateGlobal.terrainMap = terrain_map
        GameStateGlobal.gameState = GameState()
        GameStateGlobal.agentList = AgentList()
        GameStateGlobal.playerList = PlayerList()
        GameStateGlobal.projectileList = ProjectileList()
        GameStateGlobal.weaponList = WeaponList()
        GameStateGlobal.itemList = ItemList()
        GameStateGlobal.teamList = game_modes.TeamList()

        GameStateGlobal.game = game_modes.names[opts.game_mode](teams=opts.n_teams)

    @classmethod
    def init(cls):
        pass

    @classmethod
    def new_agent_id(cls):
        cls.agent_id += 1
        return cls.agent_id

    @classmethod
    def new_player_id(cls):
        cls.player_id += 1
        return cls.player_id

    @classmethod
    def new_projectile_id(cls):
        cls.projectile_id += 1
        return cls.projectile_id

    @classmethod
    def disconnect(self, connection):
        GameStateGlobal.playerList.leave(connection.player)
        if connection.player is not None:
            GameStateGlobal.agentList.destroy(connection.player.agent)


from net_server import NetServer


#from terrain_map import TerrainMap
from agents import AgentList
from players import PlayerList
from projectiles import ProjectileList
from weapons import WeaponList
from game_objects import ItemList
import game_modes

#from game_modes import TeamList

# main game state wrapper
class GameState:

    def __init__(self):
        self.time = 0

    # tick all agents
    def tick(self):
        for agent in GameStateGlobal.agentList.values():
            agent.tick()
        for projectile in GameStateGlobal.projectileList.values():
            projectile.tick()
        self.time += 1
        if self.time % 100 == 0:
            print "time= %i" % (self.time,)
