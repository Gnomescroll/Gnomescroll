'''
Initialization specific to game modes
'''

import toys
from game_state import GenericMultiObjectList

names = {
    'ctf'   :   CTF,
    'dm'    :   Deathmatch,
    'tdm'   :   TeamDeathmatch,
}

class NoTeam:

    def __init__(self):
        self.players = {}

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

class Team(NoTeam):

    def __init__(self):
        NoTeam.__init__(self)
        self.flag = None
        self.base = None
        self.create_base()

    def create_base(self):
        self.base = GameStateGlobal.itemList.create('Base', self)

    def create_flag(self):
        self.flag = GameStateGlobal.itemList.create('Flag', 1, self)

    def json(self):
        d = {
            'id'    :   self.id,
            'players':  self.players,
        }
        if self.flag is not None:
            d['flag'] = self.flag.id
        if self.base is not None:
            d['base'] = self.base.id
        return d

class TeamList(GenericMultiObjectList):

    def __init__(self):
        GenericMultiObjectList.__init__(self)
        self._metaname = 'TeamList'
        self._allow_klasses = [\
            NoTeam,
            Team,
        ])


class Game:

    def __init__(self):
        self.viewers = GameStateGlobal.teamList.create('NoTeam')

    def player_join_team(self, player, team=None):
        pass

class TeamGame(Game):

    def __init__(self, teams=2):
        Game.__init__(self)
        self.n_teams = teams
        self.teams = GameStateGlobal.teamList
        for i in xrange(n_teams):
            self.teams.create('Team')

    def player_join_team(self, player, team=None):
        if player.team is not None:
            player.team.remove_player(player)
            
        if team is None:    # default to 'viewers'
            team = self.viewers

        team.add_player(player)
        NetOut.event.player_team(player)


class CTF(TeamGame):

    def __init__(self):
        TeamGame.__init__(self, teams=2)
        for team in self.teams.values():
            team.create_flag()
        

class Deathmatch(Game):

    def __init__(self):
        Game.__init__(self)


class TeamDeathmatch(TeamGame):

    def __init__(self, teams=2):
        TeamGame.__init__(self, teams)
