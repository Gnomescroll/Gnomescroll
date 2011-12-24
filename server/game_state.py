import opts
opts = opts.opts

import math

import c_lib.terrain_map as terrain_map
import c_lib.c_lib_agents as cAgents

class GameStateGlobal:

    gameState = None
    terrainMap = None
    agentList = None
    weaponList = None

    fps = 30.
    agent_tick_mode = 'jetpack'

    def __init__(self):
        GameStateGlobal.terrainMap = terrain_map
        GameStateGlobal.gameState = GameState()
        GameStateGlobal.agentList = AgentList()
        GameStateGlobal.weaponList = WeaponList()

    @classmethod
    def init(cls):
        pass

    @classmethod
    def disconnect(self, connection):
        if connection.agent is not None:
            connection.agent.quit()
            GameStateGlobal.agentList.destroy(connection.agent)

from net_server import NetServer

from agents import AgentList
from weapons import WeaponList

# main game state wrapper
class GameState:

    def __init__(self):
        self.time = 0

    # tick all agents
    def tick(self):
        for agent in GameStateGlobal.agentList.values():
            agent.tick()
        self.time += 1
        if self.time % 100 == 0:
            print "time= %i" % (self.time,)
