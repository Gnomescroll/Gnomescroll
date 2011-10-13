
class MapControllerGlobal:
    mapController = None
    @classmethod
    def init_0(cls):
        MapControllerGlobal.mapController = MapController()
    @classmethod
    def init_1(cls):
        cls.mapController.init()

from game_state import GameStateGlobal
from net_out import NetOut

import c_lib.terrain_map as terrain_map

from profiler import P
#time.clock()
class MapController:
    #terrainMap = None
    mapMessage = None
    @classmethod
    def init(self):
        #self.terrainMap = GameStateGlobal.terrainMap
        assert NetOut.mapMessage != None
        self.mapMessage = NetOut.mapMessage
        assert self.mapMessage != None
    def __init__(self):
        self.requests = 0
        self.requests_max = 30
        self.r_total = 0 #total requests

    def process_chunk_list(self, list):
        print "Processing Chunk List: %i chunks" %(len(list))
        for (x,y,z,version) in list:
            terrain_map.set_server_version(x,y,z,version)
            #print "Chunks: %i, %i, %i" % (x,y,z)

    def tick(self):
        #print "requests= %i " % (self.requests)
        #while self.requests < self.requests_max:


        if True:
            if self.requests < self.requests_max:
                #P.event("terrain_map.chunk_request()")
                tmp = terrain_map.chunk_request()
                #P.event("map request 2")
                if tmp == None:
                    pass
                else:
                    x,y,z = tmp
                    self.send_request(x,y,z)
                pass
                #P.event("map request 3")
                #break #optional
        else:
            while self.requests < self.requests_max:
                P.event("map request 1")
                tmp = terrain_map.chunk_request()
                P.event("map request 2")
                if tmp == None:
                    pass
                else:
                    x,y,z = tmp
                    self.send_request(x,y,z)
                pass
                P.event("map request 3")
                
    def incoming_map_chunk(self, x,y,z):
        #print "map chunk received: %i, %i, %i" %(x,y,z)
        self.requests -= 1

    '''
        if self.requests.has_key((x,y,z)):
            del self.requests[(x,y,z)]
        else:
            print "MapController.incoming_map_chunk map chunk key does not exist, (%i, %i, %i)" % (x,y,z)
            assert False
    '''

    def send_request(self, x,y,z):
        self.r_total += 1
        #print "rt= %i" % (self.r_total)
        #print "map chunk request: " + str((x,y,z))
        self.requests += 1
        self.mapMessage.request_chunk(x,y,z)

