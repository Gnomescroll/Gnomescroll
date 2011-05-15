from terrain_map import TerrainMap

class WorldStateGlobal:

    terrainMap = None

    def __init__(self):
        WorldStateGlobal.terrainMap = TerrainMap()

    @classmethod
    def init(self):
        pass
