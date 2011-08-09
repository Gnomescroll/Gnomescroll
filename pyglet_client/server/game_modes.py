'''
Initialization specific to game modes
'''

import toys
from object_lists import GenericMultiObjectList


team_types = {
    1   :   'NoTeam',
    2   :   'Team',
}

class NoTeam:

    def __init__(self, id, *args, **kwargs):
        self.id = id
        self.players = {}
        self.type = 1

    def add_player(self, player):
        self.players[player.id] = player
        player.team = self
        if player.agent is not None:
            player.agent.team = self

    def remove_player(self, player):
        if player.id not in self.players:
            return
        del self.players[player.id]
        player.team = None
        if player.agent is not None:
            player.agent.team = None

    def __len__(self):
        return len(self.players)
    def __iter__(self):
        return iter(self.players)
    def items(self):
        return self.players.items()
    def values(self):
        return self.players.values()
    def keys(self):
        return self.players.keys()
    def __str__(self):
        return repr(self)
    def __repr__(self):
        return repr(self.players.keys())
    def __setitem__(self, key, value):
        self.players[key] = value
    def __getitem__(self, key):
        return self.players[key]

    def is_viewers(self):
        return True
        
    def json(self):
        return {
            'id'    :   self.id,
            'players':  self.players.keys(),
            'type'  :   self.type,
        }

class Team(NoTeam):

    def __init__(self, id, color='red', *args, **kwargs):
        NoTeam.__init__(self, id, *args, **kwargs)
        self.flag = None
        self.base = None
        self.type = 2
        self.color = color
        self.create_base()

    def create_base(self):
        self.base = GameStateGlobal.itemList.create('Base', self)

    def create_flag(self):
        self.flag = GameStateGlobal.itemList.create('Flag', 1, self)

    def is_viewers(self):
        return False

    def json(self):
        d = NoTeam.json(self)
        if self.flag is not None:
            d['flag'] = self.flag.id
        if self.base is not None:
            d['base'] = self.base.id
        d['color'] = self.color
        return d

class TeamList(GenericMultiObjectList):

    def __init__(self):
        GenericMultiObjectList.__init__(self)
        self._metaname = 'TeamList'
        self._allow_klasses([\
            NoTeam,
            Team,
        ])

class Game:

    def __init__(self):
        self.viewers = GameStateGlobal.teamList.create('NoTeam')

    def player_join_team(self, player, team=None):
        pass

    def remove_player(self, player):
        self.viewers.remove_player(player)
        
class TeamGame(Game):

    def __init__(self, teams=2, *args):
        Game.__init__(self)
        self.n_teams = teams
        self.teams = GameStateGlobal.teamList
        self.team_colors = ('red', 'blue', 'green', 'purple', 'orange', 'yellow')
        for i in xrange(self.n_teams):
            self.teams.create('Team', color=self.team_colors[i])

    def player_join_team(self, player, team=None):
        if player.team is not None:
            player.team.remove_player(player)
            
        if team is None:    # default to 'viewers'
            team = self.viewers

        team.add_player(player)
        if player.team.is_viewers():
            player.agent.dump_inventory()
        NetOut.event.player_team(player)
        player.agent.die()

    def remove_player(self, player):
        for team in self.teams.values():
            team.remove_player(player)


class CTF(TeamGame):

    def __init__(self, teams=2):
        TeamGame.__init__(self, teams=2)
        for team in self.teams.values():
            if team == self.viewers:
                continue
            team.create_flag()
        

class Deathmatch(Game):

    def __init__(self):
        Game.__init__(self)


class TeamDeathmatch(TeamGame):

    def __init__(self, teams=2):
        TeamGame.__init__(self, teams)

names = {
    'ctf'   :   CTF,
    'dm'    :   Deathmatch,
    'tdm'   :   TeamDeathmatch,
}

from game_state import GameStateGlobal
from net_out import NetOut
