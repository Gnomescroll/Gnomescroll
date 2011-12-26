import opts
opts = opts.opts

import math

class GameStateGlobal:

    gameState = None
    agentList = None

    agent = None

    fps = 30.

    apply_gravity = True

    @classmethod
    def init(cls):
        cls.agentList = AgentList()
        cls.exit = False

    @classmethod
    def remove_agent(cls, id):
        agent = cls.agentList.get(id, None)
        if agent is None:
            return
        cls.agentList.destroy(agent)

    @classmethod
    def scoreboard(cls):
        props = ['name', 'kills', 'deaths', 'score', 'id']
        defs =  [[] for i in range(len(props))]
        stats = dict(zip(props, defs))

        for agent in cls.agentList.values():
            stats['name'].append(NetClientGlobal.name)
            stats['kills'].append(agent.kills)
            stats['deaths'].append(agent.deaths)
            stats['score'].append(agent.score)
            stats['id'].append(agent.id)

        return stats

from object_lists import AgentList
from agents import Agent, PlayerAgent
from net_client import NetClientGlobal
from net_out import NetOut

import c_lib.terrain_map as TerrainMap
