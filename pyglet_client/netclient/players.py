class Player:

    def __init__(self, cid=None, id=None, name='', kills=0, deaths=0, agent=None):
        if cid is None or id is None:
            print 'Client_id or pid missing; abort creating player'
            raise ValueError
            return
        self.cid = cid
        self.id = id
        self.name = name
        print 'New player assigned name: %s' % (name,)
        self.kills = kills
        self.deaths = deaths
        if type(agent) == dict:
            print 'new Player: agent dict, creating agent'
            agent = GameStateGlobal.agentList.create(**agent)
        self.agent = agent
        self.you = False


    def tick(self):
        pass

    def update_info(self, **player):
        args = []
        if 'id' in player:
            self.id = player['id']
            args.append(self.id)
        if 'cid' in player:
            self.cid = player['cid']
        if 'name' in player:
            print 'updating player name: %s  -> %s' % (self.name, player['name'],)
            self.name = player['name']
        if 'kills' in player:
            self.kills = player['kills']
        if 'deaths' in player:
            self.deaths = player['deaths']

        GameStateGlobal.playerList.update(self, *args)

class YouPlayer(Player):

    def __init__(self):
        self.cid = NetClientGlobal.client_id
        self.name = NetClientGlobal.name
        self.id = 0
        self.kills = 0
        self.deaths = 0
        self.agent = GameStateGlobal.agentList.create_player_agent(self.id, 0)
        self.you = True
        

from game_state import GameStateGlobal
from net_client import NetClientGlobal
from agents import PlayerAgent
