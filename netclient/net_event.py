'''
Client network incoming
'''

import json
import zlib

import init_c_lib
import stats

class NetEventGlobal:
    messageHandler = None
    mapMessageHandler = None
    miscMessageHandler = None

    @classmethod
    def init(cls):
        cls.messageHandler = MessageHandler()
        cls.chatMessageHandler = ChatMessageHandler()
        cls.miscMessageHandler = MiscMessageHandler()

        cls.mapMessageHandler = MapMessageHandler()

    @classmethod
    def register_json_events(cls, events):
        for string, function in events.items():
            cls.messageHandler.json_events[string] = function

class MessageHandler:
    def __init__(self):
        self.json_events = {} #map strings to functions

    def process_net_event(self, msg_type, datagram):
        if msg_type == 1:       #json message
            self.process_json_event(msg_type, datagram)
        elif msg_type == 3:                   #create a process json message
            self.process_binary_event(msg_type, datagram)
        elif msg_type == 4:
            self.process_json_compressed_event(msg_type, datagram)
        else:
            print "WARNING: MessageHandler:: unknown msg_type %d" % (msg_type,)

#binary events
    def process_binary_event(self, msg_type, datagram):
        if msg_type == 3:
            NetEventGlobal.mapMessageHandler._map_chunk(datagram)
        elif msg_type == 4:
            raise Exception, "msg_type 4 not implemented"
        else:
            print "MessageHandler.process_binary_event: message type unknown, " + str(msg_type)

#message events
    def process_json_event(self, msg_type, datagram):
        try:
            msg = json.loads(datagram)
            #print msg
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
        #print "Received python message: %s" % (cmd,)
        if cmd in self.json_events:
            #print msg
            self.json_events[cmd](**msg)
        else:
            print "Error, received command %s that client cannot handle" % (cmd,)
            print 'msg %s' % (msg,)
            print self.json_events.keys()
            assert False

    def process_json_compressed_event(self, msg_type, datagram):
        try:
            sizea = len(datagram)
            msg = zlib.decompress(datagram)
            sizeb = len(msg)
            print "Compressed Json: compressed size= %i, uncompressed size= %i " % (sizea, sizeb)
            #f = open('./tmp', 'w')
            #f.write(msg)
        except Exception, e:
            print "MessageHandler zlib decompression failed"
            print e
            return
        self.process_json_event(msg_type, msg)

class GenericMessageHandler:

    events = {}

    def register_events(self):
        NetEventGlobal.register_json_events(self.events)

    def _assign_events_to_methods(self):
        for event, name in self.events.items():
            ev = getattr(self, str(name), None)
            if ev is not None:
                self.events[event] = ev

    def __init__(self):
        self._assign_events_to_methods()
        self.register_events()


class ChatMessageHandler(GenericMessageHandler):

    events = {
        'chat' : '_chat',
    }

    def _chat(self, **msg):
        ChatClientGlobal.chatClient.receive(msg)

class MiscMessageHandler(GenericMessageHandler):
    events = {
        'ping' : '_ping',
    }

    def _ping(self, timestamp, **msg):
        stats.last_ping = init_c_lib.get_ticks() - timestamp

class MapMessageHandler(GenericMessageHandler):

    events = {
        'chunk_list' : '_chunk_list',
        'map_chunk' : '_map_chunk',
    }

    def _chunk_list(self, list, **msg):
        print "chunk list"
        MapControllerGlobal.mapController.process_chunk_list(list)

    def _map_chunk(self, datagram):
        (x,y,z) = terrainMap.set_packed_chunk(datagram)
        MapControllerGlobal.mapController.incoming_map_chunk(x,y,z)

from net_client import NetClientGlobal
from net_out import NetOut
from chat_client import ChatClientGlobal
from map_controller import MapControllerGlobal

import c_lib.terrain_map as terrainMap

