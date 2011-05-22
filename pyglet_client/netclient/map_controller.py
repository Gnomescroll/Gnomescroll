
class MapControllerGlobal:
    mapController = None
    @classmethod
    def init_0(self):
        self.mapController = MapController()
    @classmethod
    def init_1(self):
        self.mapController.init()

from world_state import WorldStateGlobal
from net_out import NetOut

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
        ctime = time.time()
        for (x,y,z),t in self.requests.items():
            if t + self.time_out < ctime:
                self.send_request(x,y,z)

        if len(self.requests.keys()) < self.max_requests:
            self.new_request(self)

    def new_request(self):
        _x = 0
        _y = 0
        _z = 0
        list = terrainMap.get_chunk_version_list()

        min_score = 1000
        min_chunk = None
        for (x,y,z,v0,v1) in list:
            score = (x -_x+ 4)**2 + (y-_y+4)**2 + (z-_z+4)**2
            if score < min_score:
                if v0 == v1 or self.requests.has_key((x,y,z)):
                    continue
                elif self.requests.has_key((x,y,z)):
                    continue
                else:
                    min_score = score
                    min_chunk = (x,y,z)
        if min_chunk != None:
            self.requests(x,y,z) = time.time()
            self.send_request(x,y,z)

    def incoming_map_chunk(self, x,y,z):
        if self.requests.has_key((x,y,z))
            del self.requests[(x,y,z)]
        else:
            print "MapController.incoming_map_chunk map chunk key does not exist"

    def send_request(self, x,y,z):
        NetOut.MapMessage.request_chunk(x,y,z)

