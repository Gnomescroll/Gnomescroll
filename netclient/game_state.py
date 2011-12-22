import opts
opts = opts.opts

import math

class GameStateGlobal:

    gameState = None
    agentList = None
    playerList = None
    teamList = None
    game = None

    player = None
    agent = None

    fps = 30.

    apply_gravity = True

    def __init__(self):
        pass

    @classmethod
    def init_0(cls):
        cls.agentList = AgentList()
        cls.playerList = PlayerList()
        cls.weaponList = WeaponList()
        cls.teamList = game_modes.TeamList()
        cls.exit = False

    @classmethod
    def init_1(cls):
        pass

    @classmethod
    def start_game_mode(cls, mode, **kwargs):
        mode = mode.lower()
        cls.game_mode_name = mode
        cls.game = game_modes.names[mode](**kwargs)
        if opts.auto_assign_team and \
           (cls.agent.team is None or cls.agent.team == 1 or cls.agent.team.id == 1): # prevent reassignment on team list update, probably need better team adjust for team update
            team = cls.game.auto_assign_team()
            NetOut.sendMessage.join_team(team)

    # for your player
    @classmethod
    def update_your_info(cls, player):
        if cls.player is None:
            print 'Creating YouPlayer'
            cls.player = cls.playerList.join_yourself(**player)

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

        cls.game.remove_player(player)

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
            stats['score'].append(player.score)
            stats['id'].append(player.id)

        return stats

from object_lists import AgentList
from agents import Agent, PlayerAgent
from object_lists import PlayerList
from object_lists import WeaponList
import game_modes
from players import Player
from net_client import NetClientGlobal
from net_out import NetOut

#import cube_lib.terrain_map
import c_lib.terrain_map as TerrainMap
