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

    def get(self, key, default=None):
        if key in self.objects:
            return self.objects[key]
        else:
            return default

    def _add(self, *args, **kwargs):
        print args, kwargs
        object = self._object_type(*args, **kwargs)
        self.objects[object.id] = object
        print '%s: %s created; id= %s' % (self._metaname, self._itemname, object.id,)
        return object
        
    def _remove(self, obj):
        id = obj.id
        if id in self.objects:
            del self.objects[id]
            print '%s: %s removed; id= %s' % (self._metaname, self._itemname, id,)
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

    def create(self, *args, **agent):
        agent = self._add(*args, **agent)
        return agent

    def create_player_agent(self, owner, id):
        from agents import Agent, PlayerAgent
        self._object_type = PlayerAgent
        player_agent = self._add(owner=owner, id=id)
        self._object_type = Agent
        return player_agent

    def destroy(self, agent):
        self._remove(agent)
        return agent

    def update(self, agent, id=None):
        if id is not None:
            if agent.you and id not in self:
                id = 0
            old = self[id]
        else:
            old = agent
        if old.id != agent.id and old.id in self.objects:
            del self.objects[old.id]
        self.objects[agent.id] = agent

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

    def join(self, **player):
        client_id = player.get('cid', None)
        name = player.get('name', None)
        if client_id is None or name is None:
            print 'player cannot join: player missing client_id or name'
            print player
            return
        player = self._add(**player)
        self.client_ids[client_id] = player.id
        self.names[name] = client_id
        return player

    def join_yourself(self):
        from players import Player, YouPlayer
        self._object_type = YouPlayer
        player = self._add()
        self._object_type = Player
        self.client_ids[player.cid] = player.id
        self.names[player.name] = player.cid
        return player
        
    def leave(self, player):
        print 'playerlist leave'
        print player
        client_id = player.cid
        name = player.name
        if self._remove(player):
            if client_id in self.client_ids:
                del self.client_ids[client_id]
            print name
            print self.names
            if name in self.names:
                del self.names[name]
        return player

    def by_name(self, name):    # returns a client_id
        if name in self.names:
            return self.names[name]
        else:
            return 0

    def by_client(self, id):    # returns a player
        if id in self.client_ids:
            pid = self.client_ids[id]
            if pid in self.objects:
                return self.objects[pid]
            else:
                return

    def update(self, player, old_id=None, old_cid=None, old_name=None):
        print 'removing old player'
        if old_name is not None and old_name in self.names:
            print 'name= %s' % (old_name,)
            del self.names[old_name]
        if old_cid is not None and old_cid in self.client_ids:
            print 'cid= %s' % (old_cid,)
            del self.client_ids[old_cid]
        if old_id != player.id and old_id in self.objects:
            print 'id= %s' % (id,)
            del self.objects[old_id]
        self.objects[player.id] = player
        self.client_ids[player.cid] = player.id
        self.names[player.name] = player.cid
