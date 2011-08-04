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

    @classmethod
    def name_from_type(self, type):
        return team_types[type]

    def __init__(self, id, players=None, *args, **kwargs):
        self.id = id
        if players is None:
            self.players = {}
        else:
            self.load_players_list(players)
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

    # players_list would be a list of player ids
    def load_players_list(self, players):
        player_objs = [GameStateGlobal.playerList[p] for p in players]
        self.players = dict(zip(players, player_objs))
        return self.players

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

    def update_info(self, **team):
        old_id = self.id
        if 'id' in team:
            self.id = id
        if 'players' in team:
            self.load_players_list(team['players'])
        TeamList.update_info(self, old_id)

class Team(NoTeam):

    def __init__(self, id, *args, **kwargs):
        NoTeam.__init__(self, id)
        self.flag = None
        self.base = None
        self.type = 2
        #self.create_base()

    #def create_base(self):
        #self.base = GameStateGlobal.itemList.create('Base', self.id)

    #def create_flag(self):
        #self.flag = GameStateGlobal.itemList.create('Flag', 1, self.id)

    def update_info(self, **team):
        NoTeam.update_info(self, **team)
        if 'flag' in team:
            self.flag = GameStateGlobal.itemList[team['flag']]
        if 'base' in team:
            self.base = GameStateGlobal.itemList[team['base']]


class Game:

    def __init__(self, *args, **kwargs):
        self.viewers = GameStateGlobal.teamList.get_viewers()


class TeamGame(Game):

    def __init__(self, teams=2, *args, **kwargs):
        Game.__init__(self)
        self.n_teams = teams
        self.teams = GameStateGlobal.teamList

    def player_join_team(self, player, team):
        for t in self.teams.values():
            if player.id in t and t != team:
                t.remove_player(player)
                break
        team.add_player(player)


class CTF(TeamGame):

    def __init__(self, *args, **kwargs):
        TeamGame.__init__(self, teams=2)
        

class Deathmatch:

    def __init__(self, **kwargs):
        pass


class TeamDeathmatch(TeamGame):

    def __init__(self, teams=2, *args, **kwargs):
        TeamGame.__init__(self, teams)


class TeamList(GenericMultiObjectList):

    def __init__(self):
        GenericMultiObjectList.__init__(self)
        self._metaname = 'TeamList'
        self._allow_klasses([\
            NoTeam,
            Team,
        ])
        self.name_from_type = NoTeam.name_from_type

    def get_viewers(self):
        for team in self.values():
            if team_types[team.type] == 'NoTeam':
                return team

names = {
    'ctf'   :   CTF,
    'dm'    :   Deathmatch,
    'tdm'   :   TeamDeathmatch,
}


from game_state import GameStateGlobal
