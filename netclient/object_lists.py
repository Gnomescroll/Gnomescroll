# generic game object datastore
# has dictionary interface for retrieving items

from c_lib.c_lib_agents import AgentListWrapper

class GenericObjectList:

    def __init__(self):
        self.objects = {}
        self._metaname = 'GenericStateList'
        self._itemname = 'GenericObject'
        self._object_type = None
        self._wrapper = None

    def __getitem__(self, key):
        if key not in self.objects:
            print '%s: %s does not exist: id= %s' % (self._metaname, self._itemname, str(key),)
            return
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

    def __str__(self):
        return str(self.objects)

    def __repr__(self):
        return repr(self.objects)

    def _add(self, *args, **kwargs):
        obj = None
        if kwargs['id'] in self.objects:
            print '%s::id %s already exists' % (self._itemname, kwargs['id'],)
            obj = self.objects[kwargs['id']]
        else:
            obj = self._object_type(*args, **kwargs)
            self.objects[obj.id] = obj
            #print '%s: %s created; id= %s' % (self._metaname, self._itemname, object.id,)
        return obj

    def create(self, *args, **kwargs):
        return self._add(*args, **kwargs)
        
    def _remove(self, obj):
        if hasattr(obj, 'id'):
            id = obj.id
        else:
            id = obj
        if id in self.objects:
            del self.objects[id]
            #print '%s: %s removed; id= %s' % (self._metaname, self._itemname, id,)
            if self._wrapper is not None:
                self._wrapper.remove(id)
            return True
        return False

    def load_list(self, objs):
        _objs = []
        for obj in objs:
            _objs.append(self.update_or_create(**obj))
        return _objs

    def update_or_create(self, **obj):
        if 'id' not in obj:
            return
        obj_id = obj['id']
        if obj_id in self:
            o = self[obj_id]
            o.update_info(**obj)
        else:
            o = self.create(**obj)
        return o

    def load_info(self, **obj):
        if 'id' not in obj:
            return
        obj_id = obj['id']
        if obj_id not in self:
            return
        o = self[obj_id]
        o.update_info(**obj)
        return o


# override dict methods to hit down to c *list
class GenericObjectListWrapper(GenericObjectList):

    _wrapper = None

    def __init__(self):
        pass

    def update_from_list(self):
        ids = self._wrapper.ids()
        for id in ids:
            if id not in self.objects:
                obj = self._object_type(id=id)  # create object
                self.objects[id] = obj
    
    def __getitem__(self, key):
        return GenericObjectList.__getitem__(self, key)
        
    def __setitem__(self, key, value):
        self.update_from_list()
        GenericObjectList.__setitem__(self, key, value)
        
    def __delitem__(self, key):
        self.update_from_list()
        GenericObjectList.__delitem__(self, key)
        
    def __len__(self):
        self.update_from_list()
        return GenericObjectList.__len__(self)

    def __contains__(self, key):
        self.update_from_list()
        return GenericObjectList.__contains__(self, key)

    def __iter__(self):
        self.update_from_list()
        return GenericObjectList.__iter__(self)

    def keys(self):
        self.update_from_list()
        return GenericObjectList.keys(self)

    def values(self):
        self.update_from_list()
        return GenericObjectList.values(self)

    def items(self):
        self.update_from_list()
        return GenericObjectList.items(self)

    def get(self, key, default=None):
        self.update_from_list()
        return GenericObjectList.get(self, key, default)

# datastore for agents
class AgentList(GenericObjectListWrapper):

    def __init__(self):
        from agents import Agent
        GenericObjectListWrapper.__init__(self)
        GenericObjectList.__init__(self)
        self._metaname = 'AgentList'
        self._itemname = 'Agent'
        self._object_type = Agent
        self._wrapper = AgentListWrapper

    def create(self, *args, **agent):
        agent = self._add(*args, **agent)
        return agent

    def create_player_agent(self, *args, **agent):
        from agents import Agent, PlayerAgent
        self._object_type = PlayerAgent
        player_agent = self._add(*args, **agent)
        self._object_type = Agent
        print "Created python player agent"
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

    def agents_near_point(self, x,y,z, radius):
        l=[]
        for agent in self.values():
            x_, y_, z_ = agent.pos()
            r2 = float(radius)**2
            if r2 > (x_-x)**2 + (y_-y)**2 + (z_-z)**2:
                l.append(agent)
        return l


# datastore for Players
class PlayerList(GenericObjectList):

    def __init__(self):
        from players import Player
        GenericObjectList.__init__(self)
        self._metaname = 'PlayerList'
        self._itemname = 'Player'
        self._object_type = Player
        self.client_ids = {}
        #self.names = {}

    def create(self, **player):
        client_id = player.get('cid', None)
        #name = player.get('name', None)
        #if client_id is None or name is None:
        if client_id is None:
            print 'player cannot join: player missing client_id or name'
            print player
            return
        player = self._add(**player)
        if player is not None:
            self.client_ids[client_id] = player.id
            #self.names[name] = client_id
        return player

    #def identify(self, player):
        #self.names[player.name] = player.cid
        #return player
        
    def destroy(self, player):
        client_id = player.cid
        #name = player.name
        if self._remove(player):
            if client_id in self.client_ids:
                del self.client_ids[client_id]
            #if name in self.names:
                #del self.names[name]

        return player

    #def by_name(self, name):    # returns a client_id
        #if name in self.names:
            #return self.names[name]
        #else:
            #return 0

    def by_client(self, id):    # returns a player
        if id in self.client_ids:
            pid = self.client_ids[id]
            if pid in self.objects:
                return self.objects[pid]

    #def update(self, player, old_id=None, old_cid=None, old_name=None):
    def update(self, player, old_id=None, old_cid=None):
        if old_cid is not None and old_cid in self.client_ids:
            del self.client_ids[old_cid]
        if old_id != player.id and old_id in self.objects:
            del self.objects[old_id]
        #if old_name is not None and old_name in self.names:
            #if player.cid == old_cid:
                #del self.names[old_name]
        self.objects[player.id] = player
        self.client_ids[player.cid] = player.id
        #self.names[player.name] = player.cid




# for tracking multiple objects sharing the same unique identifier counts
## Differences from GenericObjectList:
##  Multiple types of objects can be tracked.
##  They must all share the same unique id counter.
##  Objects used in GenericObjectList make their own ID increment calls in instantiation
##  Objects used in this class do not make their own ID. They are assigned an id by the controller list.
class GenericMultiObjectList(GenericObjectList):

    def __init__(self):
        GenericObjectList.__init__(self)
        self.klass_index = {}               # __name__ -> Klass
        self.klass_registers = {}           # __name__ -> [id, ...]
        self._id = 0
        self.name_from_type = lambda type: ''

    def _generate_id(self):
        self._id += 1
        return self._id

    def _allow_klass(self, klass):          # use in inheriting class's __init__ to configure objects to be tracked
        self.klass_index[klass.__name__] = klass
        self.klass_registers[klass.__name__] = []

    def _allow_klasses(self, klasses):
        for klass in klasses:
            self._allow_klass(klass)

    def _add(self, klass_name, *args, **kwargs):
        self._object_type = self.klass_index[klass_name]
        obj = GenericObjectList._add(self, *args, **kwargs)
        self.klass_registers[klass_name].append(obj.id)
        self._object_type = None
        return obj

    def create(self, klass_name=None, *args, **kwargs):
        if klass_name is None:
            #if 'name' in kwargs:
                #klass_name = kwargs['name']
            #elif 'type' in kwargs:
                #klass_name = self.name_from_type(kwargs['type'])
            klass_name = self._resolve_klass_name(**kwargs)

        return self._add(klass_name, *args, **kwargs)

    def _resolve_klass_name(self, **kwargs):
        klass_name = ''
        if 'name' in kwargs:
            klass_name = kwargs['name']
        elif 'type' in kwargs:
            klass_name = self.name_from_type(kwargs['type'])
        else:
            print 'Could not resolve klass_name :: kwargs -> %s' % (kwargs,)
        return klass_name

    def _remove(self, obj):
        if hasattr(obj, 'id'):
            id = obj.id
        else:
            id = obj
            obj = self[id]
        klass_name = obj.__class__.__name__
        self.klass_registers[klass_name].remove(id)
        return GenericObjectList._remove(self, obj)

    def destroy(self, obj):
        return self._remove(obj)

    def _filter_klass(self, klass_name):
        klass_ids = self.klass_registers[klass_name]
        objs = [self[kid] for kid in klass_ids]
        return dict(zip(klass_ids, objs))

    def update(self, obj, id=None):
        if id is None:
            return
        old = self[id]

        if old.id != obj.id and old.id in self.objects:
            del self.objects[old.id]
        self.objects[obj.id] = obj

    def load_list(self, objs):
        _objs = []
        for obj in objs:
            klass_name = self.name_from_type(int(obj['type']))
            _objs.append(self.update_or_create(klass_name, **obj))
        return _objs

    def update_or_create(self, klass_name=None, **obj):
        if 'id' not in obj:
            return
        if klass_name is None:
            klass_name = self._resolve_klass_name(**obj)
        obj_id = obj['id']
        if obj_id in self:
            o = self[obj_id]
            o.update_info(**obj)
        else:
            o = self.create(klass_name, **obj)
        return o

    def load_info(self, klass_name=None, **obj):
        if 'id' not in obj:
            return
        if klass_name is None:
            klass_name = self._resolve_klass_name(**obj)
        obj_id = obj['id']
        if obj_id not in self:
            return
        o = self[obj_id]
        o.update_info(**obj)
        return o
    
class WeaponList(GenericMultiObjectList):

    def __init__(self):
        from weapons import Pick, BlockApplier, HitscanLaserGun, GrenadePouch
        GenericMultiObjectList.__init__(self)
        self._allow_klasses([ \
            Pick,
            BlockApplier,
            HitscanLaserGun,
            GrenadePouch,
        ])
        self.name_from_type = Weapon.name_from_type
        self._metaname = 'WeaponList'
        self._itemname = 'Weapon'

from weapons import Weapon

from game_objects import GameObject
