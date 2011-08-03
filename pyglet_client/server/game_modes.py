'''
Initialization specific to game modes
'''

import toys


class Team:

    def __init__(self):
        self.players = {}
        self.base = None
        self.flag = None
        self.create_base()

    def create_base(self):
        self.base = toys.Base(self.id)

    def create_flag(self):
        self.flag = toys.Flag(1, self.id)

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


class TeamGame:

    def __init__(self, teams=2):
        self.n_teams = teams
        self.teams = {}
        for i in range(teams):
            self.teams[i] = Team()


class CTF(TeamGame):

    def __init__(self):
        TeamGame.__init__(self, teams=2)
        for i in range(self.teams):
            self.teams[i].create_flag()
        

class Deathmatch:

    def __init__(self):
        pass


class TeamDeathmatch(TeamGame):

    def __init__(self, teams=2):
        TeamGame.__init__(self, teams)
