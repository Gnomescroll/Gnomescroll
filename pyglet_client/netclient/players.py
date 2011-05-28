class Player:

    def __init__(self, client_id, id=None, name='', kills=0, deaths=0, agent=None, **args):

        self.cid = client_id
        self.id = id
        self.name = name
        self.kills = kills
        self.deaths = deaths
        if type(agent) == dict:
            if self.cid == NetClientGlobal.client_id:
                self.agent = None
            else:
                agent = GameStateGlobal.agentList.create(**agent)
        else:
            self.agent = agent


    def tick(self):
        pass

    def update_info(self, **player):
        if 'id' in player:
            self.id = player['id']
        if 'cid' in player:
            self.id = player['cid']
        if 'kills' in player:
            self.kills = player['kills']
        if 'deaths' in player:
            self.deaths = player['deaths']

class YouPlayer(Player):

    def __init__(self):
        self.cid = NetClientGlobal.client_id
        self.name = NetClientGlobal.name
        self.id = 0
        self.kills = 0
        self.deaths = 0
        self.agent = PlayerAgent(self.id, 0)
        

from game_state import GameStateGlobal
from net_client import NetClientGlobal
from agents import PlayerAgent
