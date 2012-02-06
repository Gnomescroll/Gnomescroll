'''
Outgoing network messages
'''

try:
    import simplejson as json
except ImportError:
    import json
import struct
import zlib

import c_lib.terrain_map as terrain_map

class NetOut:

    event = None
    message = None

    @classmethod
    def init(cls):
        cls.event = EventOut()
        cls.message = MessageOut()

from game_state import GameStateGlobal
from net_server import NetServer

def sendJSONevent(cmd=None, zlib=0):
    def outer(f, *args, **kwargs):
        def wrapped(*args, **kwargs):
            self = args[0]
            json_data = f(*args, **kwargs)
            if not json_data:
                return
            if json_data is True:
                json_data = {}

            cmd_final = cmd # must do this reassignment due to function scoping
            if cmd_final is None:
                cmd_final = ''
            if cmd_final or 'cmd' not in json_data:
                json_data['cmd'] = cmd_final

            if not zlib:
                self.add_json_event(json_data)
            else:
                self.add_json_compressed_event(json_data, zlib)
        return wrapped
    return outer

# sends event packets to all clients
class EventOut:

    def __init__(self):
        self.event_packets = []

    def process_events(self):
        for event_packet in self.event_packets:
            for client in NetServer.connectionPool.clients_by_id.values():
                if not client.admin:
                    client.send(event_packet)
        self.event_packets = []

    def add_json_event(self, data):
        self.event_packets.append(SendMessage.get_json(data))

    def add_json_compressed_event(self, data, level):
        self.event_packets.append(SendMessage.get_json_compressed(data, level))

    @sendJSONevent('clear_map')
    def clear_map(self):
        return True

#this is global message out across the connection pool
class MessageOut:
    def __init__(self):
        pass

def sendJSON(cmd=None, zlib=0):
    def outer(f, *args, **kwargs):
        def wrapped(*args, **kwargs):
            self = args[0]
            json_data = f(*args, **kwargs)
            if json_data is None:
                json_data = {}

            cmd_final = cmd # must do this reassignment due to function scoping
            if cmd_final is None:
                cmd_final = ''
            if cmd_final != '' or 'cmd' not in json_data:
                json_data['cmd'] = cmd_final

            if not zlib:
                self.send_json(json_data)
            else:
                self.send_json_compressed(json_data, zlib)
        return wrapped
    return outer

def sendJSONCompressed(cmd=None):
    def outer(f, *args, **kwargs):
        def wrapped(*args, **kwargs):
            self = args[0]
            json_data = f(*args, **kwargs)
            if json_data is None:
                json_data = {}

            cmd_final = cmd # must do this reassignment due to function scoping
            if cmd_final is None:
                cmd_final = ''
            if cmd_final != '' or 'cmd' not in json_data:
                json_data['cmd'] = cmd_final

            self.send_compressed_json(json_data)
        return wrapped
    return outer

class SendMessage: #each connection has one of these

    fmt = '<H'

    @classmethod
    def add_prefix(cls, id, msg):
        return struct.pack(cls.fmt, id) + msg #length prefix not included in length?

    @classmethod
    def get_json(cls, d):
        return cls.add_prefix(1, json.dumps(d))

    @classmethod
    def get_json_compressed(cls, d, level=6):
        msg = json.dumps(d)
        msg = zlib.compress(msg, level) # compression level 1-9
        msg = cls.add_prefix(4, msg)    # 4 identifies msg as zlib json
        return msg

    def __init__(self, client):
        self.client = client

    def send_json(self, data):
        msg = self.get_json(data)
        self.client.send(msg)

    def send_json_compressed(self, d, zlib=6):
        msg = self.get_json_compressed(d, zlib)
        self.client.send(msg)

    @sendJSON('chunk_list', zlib=6)
    def send_chunk_list(self):
        print "Sending chunk list"
        return {
            'list' : terrain_map.get_server_chunk_list(),
        }

    def send_chunk(self, x, y, z, XXX=[0]):
        chunk_str = terrain_map.get_packed_chunk(x,y,z)
        if chunk_str != '':
            self.client.send(self.add_prefix(3, chunk_str))
        else:
            print "send chunk error: chunk id invalid, " + str((x,y,z))

    @sendJSON('ping')
    def ping(self, timestamp):
        return {
            'timestamp'   :   timestamp,
        }
