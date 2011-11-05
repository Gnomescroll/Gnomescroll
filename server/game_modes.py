'''
Initialization specific to game modes
'''

import opts
opts = opts.opts

import toys
from object_lists import GenericMultiObjectList
from utils import filter_props

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
        
    def json(self, properties=None):
        d = {
            'id'    :   self.id,
            'type'  :   self.type,
        }
        if properties is None:
            d.update({
                'players':  self.players.keys(),
            })
        else:
            d.update(filter_props(self, properties))
        return d
        

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
        self.flag = GameStateGlobal.itemList.create('Flag', self)

    def is_viewers(self):
        return False

    def json(self, properties=None):
        d = NoTeam.json(self, properties)
        if properties is None:
            if self.flag is not None:
                d['flag'] = self.flag.id
            if self.base is not None:
                d['base'] = self.base.id
            d['color'] = self.color
        else:
            d.update(filter_props(self, properties))
        for k,v in d.items():
            if v is None:
                del d[k]
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
        self.mode = 'game'

    def player_join_team(self, player, team=None):
        pass

    def remove_player(self, player):
        self.viewers.remove_player(player)

    def json(self):
        return {
            'mode'  :    self.mode,
        }
        
class TeamGame(Game):

    def __init__(self, teams=2, team_kills=False, *args):
        Game.__init__(self)
        self.mode = 'team_game'
        self.n_teams = teams
        self.teams = GameStateGlobal.teamList
        self.team_colors = ('red', 'blue', 'green', 'purple', 'orange', 'yellow')
        for i in xrange(self.n_teams):
            self.teams.create('Team', color=self.team_colors[i])
        self.team_kills = team_kills

    def player_join_team(self, player, team=None):
        if player.team is not None:
            player.team.remove_player(player)
            
        if team is None:    # default to 'viewers'
            team = self.viewers

        team.add_player(player)
        if player.team.is_viewers():
            player.agent.dump_inventory()
        NetOut.event.player_team(player)
        player.agent.die(no_score=True, custom_msg='You joined team %d' % (team.id,), respawn_ticks=0)

    def remove_player(self, player):
        for team in self.teams.values():
            team.remove_player(player)

    def json(self):
        d = Game.json(self)
        d['team_kills'] = int(self.team_kills)
        return d

class CTF(TeamGame):

    def __init__(self, teams=2, team_kills=False):
        TeamGame.__init__(self, teams, team_kills)
        self.mode = 'ctf'
        for team in self.teams.values():
            if team == self.viewers:
                team.flag = None
            else:
                team.create_flag()
                team.flag_captures = 0
        self.flag_points = 10
        self.victory_points = opts.victory_points

    def score_flag(self, agent):
        player = agent.owner
        if not hasattr(player, 'id'):
            player = GameStateGlobal.playerList[player]
        if player is None:
            print 'WARNING:: agent with unknown owner scored flag'
        print 'player scored flag'
        player.score += self.flag_points
        player.team.flag_captures += 1
        NetOut.event.player_update(player, properties='score')
        NetOut.event.team_update(player.team, properties='flag_captures')
        self.check_victory()

    def check_victory(self):
        for team in self.teams.values():
            if team == self.viewers:
                continue
            if team.flag_captures == self.victory_points:
                print 'Team %s wins!' % (team,)
                self.reset()
                break

    def reset(self):
        for team in self.teams.values():
            if team.is_viewers():
                continue
            team.flag_captures = 0
            team.flag.spawn()
            for player in team.values():
                player.agent.respawn()
        NetOut.event.send_players()
        NetOut.event.send_teams()
                
                

    def json(self):
        d = TeamGame.json(self)
        d['victory_points'] = self.victory_points
        return d

class Deathmatch(Game):

    def __init__(self):
        Game.__init__(self)
        self.mode = 'dm'


class TeamDeathmatch(TeamGame):

    def __init__(self, teams=2, team_kills=False):
        TeamGame.__init__(self, teams, team_kills)
        self.mode = 'tdm'

names = {
    'ctf'   :   CTF,
    'dm'    :   Deathmatch,
    'tdm'   :   TeamDeathmatch,
}

from game_state import GameStateGlobal
from net_out import NetOut
