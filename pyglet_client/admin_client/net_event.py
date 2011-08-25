#!/usr/bin/python

'''
Admin Client network incoming
'''

import json

class NetEventGlobal:
    messageHandler = None
    clientMessageHandler = None
    mapMessageHandler = None
    projectileMessageHandler = None
    agentMessageHandler = None
    playerMessageHandler = None
    miscMessageHandler = None
    gameModeMessageHandler = None
    itemMessageHandler = None
    weaponMessageHandler = None
    datMessageHandler = None
    
    @classmethod
    def init_0(cls):
        cls.messageHandler = MessageHandler()
        cls.clientMessageHandler = ClientMessageHandler()
        cls.playerMessageHandler = PlayerMessageHandler()
        cls.chatMessageHandler = ChatMessageHandler()
        cls.miscMessageHandler = MiscMessageHandler()

        cls.agentMessageHandler = AgentMessageHandler()
        cls.mapMessageHandler = MapMessageHandler()
        cls.projectileMessageHandler = ProjectileMessageHandler()
        cls.weaponMessageHandler = WeaponMessageHandler()

        cls.gameModeMessageHandler = GameModeMessageHandler()
        cls.itemMessageHandler = ItemMessageHandler()

        cls.datMessageHandler = DatMessageHandler()

    @classmethod
    def init_1(cls):
        MessageHandler.init()
        ClientMessageHandler.init()
        PlayerMessageHandler.init()
        ChatMessageHandler.init()

        AgentMessageHandler.init()
        MapMessageHandler.init()
        ProjectileMessageHandler.init()
        GameModeMessageHandler.init()
        ItemMessageHandler.init()
        WeaponMessageHandler.init()
        
    @classmethod
    def register_json_events(cls, events):
        for string, function in events.items():
            cls.messageHandler.json_events[string] = function

class MessageHandler:
    player = None #move this somewhere else
    @classmethod
    def init(self):
        pass
    def __init__(self):
        self.json_events = {} #map strings to functions

    def process_net_event(self, msg_type, datagram):
        if msg_type == 1:       #json message
            self.process_json_event(msg_type, datagram)
        else:                   #create a process json message
            self.process_binary_event(msg_type, datagram)

#binary events
    def process_binary_event(self, msg_type, datagram):
        if msg_type == 3:
            NetEventGlobal.mapMessageHandler._map_chunk(datagram)
        elif msg_type == 4:
            self._4_
        else:
            print "MessageHandler.process_binary_event: message type unknown, " + str(msg_type)
#message events

    def process_json_event(self, msg_type, datagram):
        try:
            msg = json.loads(datagram)
        except Exception, e:
            print "MessageHandler.process_json_event error"
            print e
            #import sys
            #sys.exit()
            print datagram
            return
        cmd = msg.get('cmd', None)
        if cmd is None:
            return
        #use json_events when possible
        cmd = str(cmd)
        if cmd in self.json_events:
            self.json_events[cmd](**msg)
        else:
            print "Error, received command %s that client cannot handle" % (cmd,)
            print 'msg %s' % (msg,)
            print self.json_events.keys()
            assert False

class GenericMessageHandler:

    events = {}

    def register_events(self):
        NetEventGlobal.register_json_events(self.events)

    def _assign_events_to_methods(self):
        for event, name in self.events.items():
            ev = getattr(self, str(name), None)
            if ev is not None:
                self.events[event] = ev

    @classmethod
    def init(cls):
        pass
    def __init__(self):
        self._assign_events_to_methods()
        self.register_events()


class MiscMessageHandler(GenericMessageHandler):
    events = {
        'ping' : '_ping',
    }

    @classmethod
    def init(cls):
        pass

    def _ping(self, timestamp, **msg):
        stats.last_ping = SDL.gl.get_ticks() - timestamp
        #print "timestamp = %f" % (SDL.gl.get_ticks() - timestamp)

class MapMessageHandler(GenericMessageHandler):
    #terrainMap = None
    #mapChunkManager = None
    mapController = None

    events = {
        'chunk_list' : '_chunk_list',
        'map_chunk' : '_map_chunk',
        'set_map' : '_set_map',
    }

    @classmethod
    def init(cls):
        #cls.terrainMap = GameStateGlobal.terrainMap
        #cls.mapChunkManager = MapChunkManagerGlobal.mapChunkManager
        cls.mapController = MapControllerGlobal.mapController
        assert cls.mapController != None

    def _chunk_list(self, list, **msg):
        #print str(list)
        print "chunk list"
        self.mapController.process_chunk_list(list)
        #for chunk in list:
        #    (x,y,z,version ) = chunk

    def _map_chunk(self, datagram):
        #print "Map Chunk Received"
        (x,y,z) = terrainMap.set_packed_chunk(datagram)
        self.mapController.incoming_map_chunk(x,y,z)
        #self.mapChunkManager.set_map(x,y,z) #tells to redraw chunk

    def _set_map(self, list, **msg):
        if len(list) == 1:
            print 'settings block %s' % list[0]
        for x,y,z,value in list:
            terrainMap.set(x,y,z,value)
            #self.mapChunkManager.set_map(x,y,z) #redraw chunk


class ClientMessageHandler(GenericMessageHandler):

    events = {
        'client_id' : '_client_id',
        'set_client_id' : '_set_client_id',
        'client_quit' : '_client_quit',
    }

    def _client_id(self, **msg):
        if self._set_client_id(**msg):
            NetOut.sendMessage.identify()
        else:
            NetOut.sendMessage.request_client_id()

    def _set_client_id(self, id, **arg):
        print "Received Client Id: %s" % (id,)
        NetClientGlobal.client_id = id
        return True

    def _client_quit(self, id, **msg):
        GameStateGlobal.client_quit(id)
        

from net_client import NetClientGlobal
from net_out import NetOut
