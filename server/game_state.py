import opts
opts = opts.opts

import math

import c_lib.terrain_map as terrain_map
import c_lib.c_lib_agents as cAgents

class GameStateGlobal:

    agentList = None
    weaponList = None

    fps = 30.
    agent_tick_mode = 'jetpack'

    def __init__(self):
        GameStateGlobal.agentList = AgentList()
        GameStateGlobal.weaponList = WeaponList()

    @classmethod
    def disconnect(cls, connection):
        if connection.agent is not None:
            GameStateGlobal.agentList.destroy(connection.agent)

from net_server import NetServer

from agents import AgentList
from weapons import WeaponList
