# generic game object datastore
# has dictionary interface for retrieving items
class GenericObjectList:

    def __init__(self):
        self.objects = {}
        self._metaname = 'GenericStateList'
        self._itemname = 'GenericObject'
        self._object_type = None

    def __getitem__(self, key):
        if not key in self.objects:
            print '%s: %s does not exist: id= %s' % (self._metaname, self._itemname, str(key),)
        return self.objects[key]
        
    def __setitem__(self, key, value):
        self.objects[key] = value
        
    def __delitem__(self, key):
        del self.objects[key]
        
    def __len__(self):
        return len(self.objects)

    def __contains__(self, key):
        return key in self.objects

    def __iter__(self):
        return iter(self.objects)

    def keys(self):
        return self.objects.keys()

    def values(self):
        return self.objects.values()

    def items(self):
        return self.objects.items()

    def _add(self, *args):
        print args
        object = self._object_type(*args)
        self.objects[object.id] = object
        print '%s: %s created; id= %i' % (self._metaname, self._itemname, object.id,)
        return object
        
    def _remove(self, id):
        if type(id) != int:
            id = id.id
        if id in self.objects:
            del self.objects[id]
            print '%s: %s removed; id= %i' % (self._metaname, self._itemname, object.id,)
            return True
        return False

# datastore for agents
class AgentList(GenericObjectList):

    def __init__(self):
        from agents import Agent
        GenericObjectList.__init__(self)
        self._metaname = 'AgentList'
        self._itemname = 'Agent'
        self._object_type = Agent

    def create(self, player_id, x=0, y=0, z=0, xa=0, ya=0):
        self._add(x, y, z, xa, ya, player_id)

    def destroy(self, agent):
        self._remove(agent)

# datastore for Players
class PlayerList(GenericObjectList):

    def __init__(self):
        from players import Player
        GenericObjectList.__init__(self)
        self._metaname = 'PlayerList'
        self._itemname = 'Player'
        self._object_type = Player
        self.client_ids = {}
        self.names = {}

    def join(self, client_id, name):
        player = self._add(client_id, name)
        self.client_ids[client_id] = player.id
        self.names[name] = client_id
        return player
        
    def leave(self, player):
        client_id = player.client_id
        if self._remove(player) and client_id in self.client_ids:
            del self.client_ids[client_id]
        return player

    def by_name(self, name):    # returns a client_id
        if name in self.names:
            return self.names[name]
        else:
            return 0
