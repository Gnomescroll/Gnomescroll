
class MapControllerGlobal:
    mapController = None
    @classmethod
    def init_0(self):
        self.mapController = MapController()
    @classmethod
    def init_1(self):
        self.mapController.init()

from world_state import WorldStateGlobal

import time


#time.clock()
class MapController:
    terrainMap = None
    @classmethod
    def init(self):
        self.terrainMap = WorldStateGlobal.terrainMap
    def __init(self):
        self.requests = {}
        self.max_requests = 5
        self.time_out = 4.0 #needs to be adjusted

    def proces_chunk_list(self, list)
        for (x,y,z,version) in list:
            self.terrainMap.set_server_version(x,y,z,version)

    def tick(self):
        if len(self.requests.keys()) < self.max_requests:
            self.new_request(self)

    def incoming_map_chunk(self, x,y,z):
        del self.requests[(x,y,z)]

    def send_map_chunk_request(self):
        pass
