from terrain_map import TerrainMap

from player import Player

class WorldStateGlobal:

    terrainMap = None

    @classmethod
    def init_0(self):
        WorldStateGlobal.terrainMap = TerrainMap()
        WorldStateGlobal.Player = Player()
    @classmethod
    def init_1(self):
        pass
