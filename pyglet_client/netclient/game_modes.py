'''
Initialization specific to game modes
'''

import toys


class TeamGame:

    def __init__(self, teams=2):
        self.n_teams = teams
        self.teams = {}
        for i in range(teams):
            self.teams[i] = {
                'base'  :   toys.Base(),
            }


class CTF(TeamGame):

    def __init__(self):
        TeamGame.__init__(self, teams=2)
        for i in range(self.teams):
            self.teams[i]['flag'] = toys.Flag()
        
    

class Deathmatch:

    def __init__(self):
        pass


class TeamDeathmatch(TeamGame):

    def __init__(self, teams=2):
        TeamGame.__init__(self, teams)
