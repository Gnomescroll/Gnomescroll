from terrain_map import TerrainMap

from player import Player
class WorldStateGlobal:

    terrainMap = None

    def __init__(self):
        WorldStateGlobal.terrainMap = TerrainMap()
        WorldStateGlobal.Player = Player()
    @classmethod
    def init(self):
        pass
