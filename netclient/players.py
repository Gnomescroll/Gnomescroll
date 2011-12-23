
class Player(object):

    def __init__(self, cid=None, id=None, agent=None):
        if cid is None or id is None:
            print 'Client_id or pid missing; abort creating player'
            raise ValueError
            return
        self.cid = cid
        self.id = id
        self.agent = None
        self.you = False

    def __getattribute__(self, k):
        v = object.__getattribute__(self, k)
        if k == 'team':
            v = cGame.get_team(v)
        return v

    def update_info(self, **player):
        args = {}
        if 'id' in player:
            args['old_id'] = self.id
            self.id = player['id']
        if 'cid' in player:
            args['old_cid'] = self.cid
            self.cid = player['cid']

        GameStateGlobal.playerList.update(self, **args)

from game_state import GameStateGlobal
from net_client import NetClientGlobal
