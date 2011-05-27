'''
Outgoing network messages
'''

import simplejson as json
import struct


class NetOut:

    event = None
    message = None

    def __init__(self):
        NetOut.event = EventOut()
        NetOut.message = MessageOut()

    @classmethod
    def init(cls):
        cls.event.init()
        cls.message.init()

from game_state import GameStateGlobal
from net_server import NetServer

# sends event packets to all clients
class EventOut:

    def init(self):
        pass

    def __init__(self):
        self.event_packets = []

    def process_events(self):
        #print "Process Events.num_events = %i" % len(self.event_packets)
        for event_packet in self.event_packets:
            for client in NetServer.connectionPool._client_pool.values():
                client.send(event_packet)
        self.event_packets = []

    def add_json_event(self, dict):
        self.event_packets.append(SendMessage.get_json(dict))

    def agent_state_change(self, agent):
        d = {
            'cmd'  : 'agent_position',
            'id'   : agent.id,
            'tick' : GameStateGlobal.gameState.time,
            'state': agent.state #is a 9 tuple
        }
        self.add_json_event(d)

#this is global message out across the connection pool
class MessageOut:
    def init(self):
        pass
    def __init__(self):
        pass


class SendMessage: #each connection has one of these
    @classmethod
    def add_prefix(self, id, msg):
        return struct.pack('I H', 4+2+len(msg), id) + msg #length prefix not included in length?
    @classmethod
    def get_json(self, dict):
        return self.add_prefix(1, json.dumps(dict))
    def __init__(self, client):
        self.client = client
    def send_json(self, dict):
        self.client.send(self.add_prefix(1, json.dumps(dict)))

    ## messages go out immediately
    def send_client_id(self, connection):
        print "Send client id"
        d = {
            'cmd'  : 'client_id',
            'id'   : connection.id,
        }
        self.send_json(d)

    def identify_fail(self, connection, notes=''):
        print notes
        d = {
            'cmd'   : 'identify_fail',
            'msg'   : notes,
        }
        self.send_json(d)

    def identified(self, connection):
        print 'Identified'
        d = {
            'cmd'   : 'identified',
            'player': connection.player.json(),
        }
        self.send_json(d)
        
    def send_chunk_list(self):
        d = {
            'cmd'  : 'chunk_list',
            'list' : GameStateGlobal.terrainMap.get_chunk_list(),
        }
        self.send_json(d)

    def send_chunk(self, x, y, z):
        chunk_str = GameStateGlobal.terrainMap.get_packed_chunk(x,y,z)
        if chunk_str != '':
            self.client.send(self.add_prefix(3, chunk_str))
        else:
            print "send chunk error: chunk id invalid, " + str((x,y,z))
