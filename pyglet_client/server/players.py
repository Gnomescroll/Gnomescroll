from game_state import GenericObjectList
from game_state import GameStateGlobal
from net_out import NetOut

# datastore for Players
class PlayerList(GenericObjectList):

    def __init__(self):
        GenericObjectList.__init__(self)
        self._object_type = Player
        self.client_ids = {}
        self.names = {}

    def join(self, client_id, name):
        player = self._add(client_id, name)
        self.client_ids[client_id] = player.id
        self.names[name] = player.cid
        GameStateGlobal.game.player_join_team(player)
        return player
        
    def leave(self, player):
        try:
            client_id = player.cid
            name = player.name
        except AttributeError:
            return
        if self._remove(player):
            if client_id in self.client_ids:
                del self.client_ids[client_id]
            if name in self.names:
                del self.names[name]
        return player

    def client(self, client_id):
        return self[self.client_ids[client_id]]

    def by_name(self, name):    # returns a client_id
        if name in self.names:
            return self.names[name]
        else:
            return 0

    def update(self, player, old_id=None, old_cid=None, old_name=None):
        if old_id != player.id and old_id in self:
            del self.objects[old_id]
        if old_cid is not None and old_cid in self.client_ids:
            del self.client_ids[old_cid]
        if old_name is not None and old_name in self.names:
            del self.names[old_name]
        self.objects[player.id] = player
        self.client_ids[player.cid] = player.id
        self.names[player.name] = player.cid
                
# represents a "Player" (player score, agents they control etc)
class Player:

    def __init__(self, client_id, name, id=None, team=None):
        self.kills = 0
        self.deaths = 0
        self.name = name
        self.cid = client_id
        if id is None:
            id = GameStateGlobal.new_player_id()
        self.id = id
        self.agent = GameStateGlobal.agentList.create(self.id)
        self.team = 1

    def json(self, properties=None, agent_properties=None): # json encodable string representation
        d = {
            'id'    : self.id,
        }
        if properties is None:
            d.update({
            'kills' : self.kills,
            'deaths': self.deaths,
            'name'  : self.name,
            'cid'   : self.cid,
            'agent' : '',
            'team'  : self.team.id,
        })
        else:
            if type(properties) == str:
                properties = [properties]
            for prop in properties:
                if prop == 'team':
                    d[prop] = self.team.id
                else:
                    d[prop] = getattr(self, prop)
        if 'agent' in d:
            d['agent'] = self.agent.json(agent_properties)
        return d    

    def killed(self):
        self.kills += 1
        NetOut.event.player_update(self, 'kills')

    def died(self):
        self.deaths += 1
        NetOut.event.player_update(self, 'deaths')

    def quit(self):
        GameStateGlobal.playerList.leave(self)

    def owns(self, agent):
        return (self.agent == agent)

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

        GameStateGlobal.playerList.update(self, **args)
