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
    # this is *not* playerList, this is a list of player id's sent in the teams' json
    def load_players_list(self, players):
        player_objs = []
        for p in players:
            _p = GameStateGlobal.playerList[p]
            if _p is None:
                player_objs.append(p)
            else:
                player_objs.append(_p)
        self.players = dict(zip(players, player_objs))
        return self.players

    # assigns player_ids to player objects if available.
    # need to call after receiving a playerList update
    def update_players_list(self):
        for pid in self:
            _p = GameStateGlobal.playerList[pid]
            if _p is None:
                self[pid] = pid
            else:
                self[pid] = _p

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

    def update_info(self, **team):
        old_id = self.id
        if 'id' in team:
            self.id = id
        if 'players' in team:
            self.load_players_list(team['players'])
        TeamList.update_info(self, old_id)

class Team(NoTeam):

    def __init__(self, id, color=None, *args, **kwargs):
        NoTeam.__init__(self, id)
        self.flag = None
        self.base = None
        self.type = 2
        self.color = color

    def update_info(self, **team):
        NoTeam.update_info(self, **team)
        if 'flag' in team:
            self.flag = GameStateGlobal.itemList[team['flag']]
        if 'base' in team:
            self.base = GameStateGlobal.itemList[team['base']]
        if 'color' in team:
            self.color = team['color']


class Game:

    def __init__(self, *args, **kwargs):
        self.viewers = GameStateGlobal.teamList.get_viewers()

    def remove_player(self, player):
        self.viewers.remove_player(player)

    def update_players(self):
        for team in GameStateGlobal.teamList.values():
            team.update_players_list()


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

    def remove_player(self, player):
        for team in self.teams.values():
            team.remove_player(player)

    def auto_assign_team(self):
        return self.smallest_team()

    def smallest_team(self):
        sorted_t = sorted(self.teams.values(), key=len)
        smallest = sorted_t[0]
        if smallest.type == 1: # viewers, skip this team
            smallest = sorted_t[1]
        return smallest



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
