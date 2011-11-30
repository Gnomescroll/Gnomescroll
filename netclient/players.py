
class Player:

    def __init__(self, cid=None, id=None, name='', kills=0, deaths=0, suicides=0, agent=None, team=None, score=0):
        if cid is None or id is None:
            print 'Client_id or pid missing; abort creating player'
            raise ValueError
            return
        self.cid = cid
        self.id = id
        self.name = name
        self.team = team
        print 'New player assigned name: %s' % (name,)
        self.kills = kills
        self.deaths = deaths
        if type(agent) == dict:
            print 'new Player: agent dict, creating agent'
            print "^ lying"
            #agent = GameStateGlobal.agentList.create(**agent)
        #self.agent = agent
        #self.agent = agent['id']
        self.agent = None
        self.you = False
        self.score = score
        self.suicides = suicides # use this later

    def tick(self):
        pass

    def update_info(self, **player):
        args = {}
        if 'id' in player:
            args['old_id'] = self.id
            self.id = player['id']
        if 'cid' in player:
            args['old_cid'] = self.cid
            self.cid = player['cid']
        if 'name' in player:
            print 'updating player name: %s  -> %s' % (self.name, player['name'],)
            args['old_name'] = self.name
            self.name = player['name']
        if 'kills' in player:
            self.kills = player['kills']
        if 'deaths' in player:
            self.deaths = player['deaths']
        if 'team' in player:
            self.team = GameStateGlobal.teamList[player['team']]
        if 'score' in player:
            self.score = player['score']

        GameStateGlobal.playerList.update(self, **args)

class YouPlayer(Player):

    def __init__(self, cid=None, id=None, name='', kills=0, deaths=0, suicides=0, agent=None, team=None, score=0):
        self.cid = cid
        self.name = name
        self.id = id
        self.kills = kills
        self.deaths = deaths
        #self.agent = agent['id']
        self.agent = None
        self.you = True
        self.suicides = suicides
        self.team = team
        self.score = score
        

from game_state import GameStateGlobal
from net_client import NetClientGlobal
