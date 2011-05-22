#!/usr/bin/python

'''
Client network incoming
'''

import simplejson as json
#import struct

from chat_client import ChatClientGlobal

class NetEventGlobal:
    messageHandler = None
    mapMessageHandler = None
    @classmethod
    def init_0(self):
        self.messageHandler = MessageHandler()  ##MAY CAUSE ERRORS?
        self.mapMessageHandler = MapMessageHandler()
    @classmethod
    def init_1(self):
        MessageHandler.init()

from world_state import WorldStateGlobal
from net_client import NetClientGlobal
from net_out import NetOut
from chat_client import ChatClientGlobal
from map_chunk_manager import MapChunkManagerGlobal

class MessageHandler:
    player = None #move this somewhere else
    @classmethod
    def init(self):
        self.player = WorldStateGlobal.player
        assert self.player != None
    def __init__(self):
        pass

    def process_net_event(self, msg_type, datagram):
        if msg_type == 1:       #json message
            self.process_json_event(msg_type, datagram)
        else:                   #create a process json message
            self.process_binary_event(msg_type, datagram)

#binary events
    def process_binary_event(self, msg_type, datagram):
        if msg_type == 3:
            NetEventGlobal.mapMessageHandler._map_chunk(datagram)
        if msg_type == 4:
            self._4_
        else:
            print "MessageHandler.process_binary_event: message type unknown"
#message events

    def process_json_event(self, msg_type, datagram):
        try:
            msg = json.loads(datagram)
        except:
            print "MessageHandler.process_json_event error"
            print str(msg)
            return
        cmd = msg.get('cmd', None)
        if cmd is None:
            return
        if cmd == 'agent_position':
            self._agent_position(**msg)
        elif cmd == 'send_client_id':
            self._set_client_id(**msg)
        #map events
        elif cmd == 'chunk_list':
            NetEventGlobal.mapMessageHandler._chunk_list(**msg)
            print "Chunk List Received"
            print str(msg['list'])
        elif cmd == 'set_map':
            NetEventGlobal.mapMessageHandler._set_map(**msg)
        elif cmd == 'chat':
            ChatClientGlobal.chatClient.receive(msg)
        else:
            print "JSON message type unregonized"

    def _agent_position(self, id, tick, state, **misc):
        self.player = WorldStateGlobal.player
        [x,y,z,vx, vy, vz,ax, ay, az] = state
        [x,y,z] = [float(x),float(y),float(z)]

        self.player.x = x
        self.player.y = y
        self.player.z = z
        self.player.vx = vx
        self.player.vy = vy
        self.player.vz = vz
        self.player.ax = ax
        self.player.ay = ay
        self.player.az = az

    def _set_client_id(self, id, **misc):
        print "Received Client Id: %i" % (id,)
        if NetClientGlobal.client_id == 0:
            NetClientGlobal.client_id = id
            NetOut.sendMessage.send_client_id()
            #app.mainLoop()
            ChatClientGlobal.on_register()

class MapMessageHandler:
    terrainMap = None
    mapChunkManager = None
    mapController = None
    @classmethod
    def init(self):
        self.terrainMap = WorldStateGlobal.terrainMap
        self.cubeGlobal = CubeGlobal.mapChunkManager
        self.mapController = MapControllerGlobal.mapController
    def __init__(self):
        pass

    def _chunk_list(self, list, **msg):
        self.mapController.proces_chunk_list(list)
        #for chunk in list:
        #    (x,y,z,version ) = chunk

    def _map_chunk(self, datagram):
        print "Map Chunk Received"
        (x,y,z) = self.terrainMap.set_packed_chunk(datagram)
        self.mapChunkManager.set_map(x,y,z) #tells to redraw chunk
        self.mapController.incoming_map_chunk(x,y,z)

    def _set_map(self, value, **msg):
        (x,y,z,value) = value
        self.terrainMap.set(x,y,z,value)
        self.mapChunkManager.set_map(x,y,z) #redraw chunk
