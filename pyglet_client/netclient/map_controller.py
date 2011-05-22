
class MapControllerGlobal:
    mapController = None
    @classmethod
    def init_0(self):
        MapControllerGlobal.mapController = MapController()
    @classmethod
    def init_1(self):
        MapControllerGlobal.mapController.init()

from world_state import WorldStateGlobal
from net_out import NetOut

import time


#time.clock()
class MapController:
    terrainMap = None
    mapMessage = None
    @classmethod
    def init(self):
        self.terrainMap = WorldStateGlobal.terrainMap
        assert NetOut.mapMessage != None
        self.mapMessage = NetOut.mapMessage
        assert self.mapMessage != None
    def __init__(self):
        self.requests = {}
        self.max_requests = 5
        self.time_out = 4.0 #needs to be adjusted

    def process_chunk_list(self, list):
        for (x,y,z,version) in list:
            self.terrainMap.set_server_version(x,y,z,version)

    def tick(self):
        ctime = time.time()
        for (x,y,z),t in self.requests.items():
            if t + self.time_out < ctime:
                self.requests[(x,y,z)] = ctime
                self.send_request(x,y,z)

        if len(self.requests.keys()) <= self.max_requests:
            self.new_request()

    def new_request(self):
        #print "Start Map Chunk Request"
        _x = 0
        _y = 0
        _z = 0
        list = self.terrainMap.get_chunk_version_list()
        #print len(list)
        min_score = 100000
        min_chunk = None
        min_v = None
        for (x,y,z,v0,v1) in list:
            score = (x -_x+ 4)**2 + (y-_y+4)**2 + (z-_z+4)**2
            if score < min_score:
                if v0 == v1:
                    continue
                elif self.requests.has_key((x,y,z)):
                    continue
                else:
                    #print str((x,y,z, v0, v1))
                    min_score = score
                    min_chunk = (x,y,z)
                    min_v = (v0, v1)
        if min_chunk != None:
            print "w: " + str(min_chunk) + " " + str(min_v)
            assert not self.requests.has_key(min_chunk)
            self.requests[(min_chunk)] = time.time()
            self.send_request(*min_chunk)

    def incoming_map_chunk(self, x,y,z):
        print "incoming map chunk: " + str((x,y,z))
        if self.requests.has_key((x,y,z)):
            del self.requests[(x,y,z)]
        else:
            print "MapController.incoming_map_chunk map chunk key does not exist"

    def send_request(self, x,y,z):
        print "map chunk request: " + str((x,y,z))
        self.mapMessage.request_chunk(x,y,z)

