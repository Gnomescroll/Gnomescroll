import opts
opts = opts.opts

import math

class GameStateGlobal:

    gameState = None
    agentList = None
    #playerList = None
    game = None

    #player = None
    agent = None

    fps = 30.

    apply_gravity = True

    @classmethod
    def init_0(cls):
        cls.agentList = AgentList()
        #cls.playerList = PlayerList()
        cls.weaponList = WeaponList()
        cls.exit = False

    @classmethod
    def init_1(cls):
        pass

    # do something abt this
    @classmethod
    def update_your_info(cls, player, data):
        print data
        if cls.player is None and player.cid == NetClientGlobal.connection.client_id:
            print "Assigned GameStateGlobal.player"
            cls.player = player

            if cls.agent is None:
                agent = data['player'].get('agent', None)
                if agent is not None:
                    cls.agent = GameStateGlobal.agentList.create_player_agent(**agent)
                    print 'Assigned GameStateGlobal.agent'
                    print str(cls.agent)
                    weapons = agent.get('weapons', None)
                    if weapons is not None:
                        cls.agent.weapons.update_info(**weapons)

            cls.player.agent = cls.agent
            
        return cls.player

    #@classmethod
    #def remove_player(cls, id):
        #player = cls.playerList.get(id, None)
        #if player is None:
            #return
        #cls.playerList.destroy(player)

        #agent = player.agent
        #if agent is not None:
            #cls.remove_agent(agent.id, seek=False)

        #cls.game.remove_player(player)

    @classmethod
    #def remove_agent(cls, id, seek=True):
    def remove_agent(cls, id):
        agent = cls.agentList.get(id, None)
        if agent is None:
            return
        cls.agentList.destroy(agent)

        for weapon in agent.weapons:
            cls.remove_weapon(weapon, seek=False)

        #if seek:
            #owner = cls.playerList.get(agent.owner, None)
            #if owner is not None:
                #owner.agent = None

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

    #@classmethod
    #def client_quit(cls, id):
        #player = cls.playerList.by_client(id)
        #if player is None:
            #return
        #cls.remove_player(player.id)

    @classmethod
    def scoreboard(cls):
        #props = ['name', 'kills', 'deaths', 'score', 'id']
        props = ['id']
        defs =  [[] for i in range(len(props))]
        stats = dict(zip(props, defs))

        for agent in cls.agentList.values():
            #stats['name'].append(player.name)
            #stats['kills'].append(player.kills)
            #stats['deaths'].append(player.deaths)
            #stats['score'].append(player.score)
            #stats['id'].append(player.id)
            stats['id'].append(agent.id)

        return stats

from object_lists import AgentList
from agents import Agent, PlayerAgent
#from object_lists import PlayerList
from object_lists import WeaponList
#from players import Player
from net_client import NetClientGlobal
from net_out import NetOut

import c_lib.terrain_map as TerrainMap
