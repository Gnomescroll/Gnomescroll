import math

class GameStateGlobal:

    TICK = 0.01 # seconds

    gameState = None
    terrainMap = None
    agentList = None
    playerList = None
    #state
    agent_id = 0
    player_id = 0
    projectile_id = 0

    def __init__(self):
        GameStateGlobal.terrainMap = TerrainMap()
        GameStateGlobal.gameState = GameState()
        GameStateGlobal.agentList = AgentList()
        GameStateGlobal.playerList = PlayerList()
        GameStateGlobal.projectileList = ProjectileList()

    @classmethod
    def init(cls):
        pass

    @classmethod
    def new_agent_id(cls):
        cls.agent_id += 1
        return cls.agent_id

    @classmethod
    def new_player_id(cls):
        cls.player_id += 1
        return cls.player_id
        
    @classmethod
    def new_projectile_id(cls):
        cls.projectile_id += 1
        return cls.projectile_id

    @classmethod
    def disconnect(self, connection):
        GameStateGlobal.playerList.leave(connection.player)
        if connection.player is not None:
            GameStateGlobal.agentList.destroy(connection.player.agent)
    

from net_server import NetServer

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

    def __str__(self):
        return str(self.objects)

    def _add(self, *args, **kwargs):
        print args
        object = self._object_type(*args, **kwargs)
        self.objects[object.id] = object
       # print '%s: %s created; id= %s' % (self._metaname, self._itemname, object.id,)
        return object

    def _remove(self, obj):
        id = obj.id
        if id in self.objects:
            del self.objects[id]
        #    print '%s: %s removed; id= %s' % (self._metaname, self._itemname, id,)
            return True
        return False

from terrain_map import TerrainMap
from agents import AgentList
from players import PlayerList
from projectiles import ProjectileList

# main game state wrapper
class GameState:

    def __init__(self):
        self.time = 0

    # tick all agents
    def tick(self):
        for agent in GameStateGlobal.agentList.values():
            agent.tick()
        for projectile in GameStateGlobal.projectileList.values():
            projectile.tick()
        self.time += 1
        if self.time % 100 == 0:
            print "time= %i" % (self.time,)
