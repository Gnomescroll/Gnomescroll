from terrain_map import TerrainMap

from player import Player

class WorldStateGlobal:

    terrainMap = None
    player = None #need player list
    @classmethod
    def init_0(self):
        self.terrainMap = TerrainMap()
        self.player = Player()
    @classmethod
    def init_1(self):
        pass
