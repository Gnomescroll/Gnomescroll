'''
Initialization specific to game modes
'''

import toys

game_mode_names = {
    'ctf'   :   CTF,
    'dm'    :   Deathmatch,
    'tdm'   :   TeamDeathmatch,
}


class Team:

    def __init__(self):
        self.players = {}
        self.flag = None
        self.base = None
        self.create_base()

    def create_base(self):
        self.base = toys.Base(self.id)

    def create_flag(self):
        self.flag = toys.Flag(1, self.id)


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


class TeamList(GenericObjectList):

    def __init__(self):
        GenericObjectList.__init__(self)
        self._metaname = 'TeamList'
        self._itemname = 'Team'
        self._object_type = Team

from object_lists import GenericObjectList
