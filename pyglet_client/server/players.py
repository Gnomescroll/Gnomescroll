from game_state import GenericObjectList
from game_state import GameStateGlobal

# datastore for Players
class PlayerList(GenericObjectList):

    def __init__(self):
        GenericObjectList.__init__(self)
        self._metaname = 'PlayerList'
        self._itemname = 'Player'
        self._object_type = Player
        self.client_ids = {}

    def join(self, client_id, name):
        player = self._add(client_id, name)
        self.client_ids[client_id] = player.id
        return player
        
    def leave(self, player):
        client_id = player.cid
        if self._remove(player) and client_id in self.client_ids:
            del self.client_ids[client_id]
        return player

    def client(self, client_id):
        print 'plyaerlist.client'
        print client_id
        print self.client_ids
        print self
        return self[self.client_ids[client_id]]

    def json(self):
        players = []
        for player in self.objects.values():
            players.append(player.json())
        return players
        
                
# represents a "Player" (player score, agents they control etc)
class Player:

    def __init__(self, client_id, name, id=None):
        self.kills = 0
        self.deaths = 0
        self.name = name
        self.client_id = client_id
        if id is None:
            id = GameStateGlobal.new_player_id()
        self.id = id
        self.agent = GameStateGlobal.agentList.create(self.id)

    def json(self): # json encodable string representation
        d = {
            'kills' : self.kills,
            'deaths': self.deaths,
            'name'  : self.name,
            'cid'    : self.client_id,
            'id'   : self.id,
            'agent' : self.agent.json()
        }
        return d    

    def killed(self):
        self.kills += 1

    def died(self):
        self.deaths += 1

    def quit(self):
        GameStateGlobal.playerList.leave(self)
