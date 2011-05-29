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


def sendJSONevent(f):
    def wrapped(*args):
        self = args[0]
        self.add_json_event(f(*args))
    return wrapped

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

    @sendJSONevent
    def agent_state_change(self, agent):
        return {
            'cmd'  : 'agent_position',
            'id'   : agent.id,
            'tick' : GameStateGlobal.gameState.time,
            'state': agent.state #is a 9 tuple
        }

    @sendJSONevent
    def player_update(self, player):
        return {
            'cmd'   : 'player_update',
            'player': player.json(),
        }

    @sendJSONevent
    def player_rename(self, player):
        return {
            'cmd'   : 'player_update',
            'player': {
                'name'  : player.name,
                'id'    : player.id,
            },
        }
            
    @sendJSONevent
    def player_join(self, player):
        return {
            'cmd'   : 'player_info',
            'player': player.json(),
        }

    @sendJSONevent
    def client_quit(self, client_id):
        return {
            'cmd'   : 'client_quit',
            'id'    : client_id,
        }
        

#this is global message out across the connection pool
class MessageOut:
    def init(self):
        pass
    def __init__(self):
        pass
        
# calls send_json
def sendJSON(f):
    def wrapped(*args):
        self = args[0]
        self.send_json(f(*args))
    return wrapped

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
    @sendJSON
    def send_client_id(self, connection):
        print "Send client id"
        return {
            'cmd'  : 'client_id',
            'id'   : connection.id,
        }

    @sendJSON
    def client_quit(self, client_id):
        return {
            'cmd'   : 'client_quit',
            'id'    : client_id,
        }

    @sendJSON
    def identify_fail(self, connection, notes=''):
        print notes
        return {
            'cmd'   : 'identify_fail',
            'msg'   : notes,
        }

    @sendJSON
    def identified(self, connection, msg=''):
        print 'Identified'
        return {
            'cmd'   : 'identified',
            'msg'   : msg,
            'player': connection.player.json(),
        }

    @sendJSON
    def send_chunk_list(self):
        return {
            'cmd'  : 'chunk_list',
            'list' : GameStateGlobal.terrainMap.get_chunk_list(),
        }

    def send_chunk(self, x, y, z):
        chunk_str = GameStateGlobal.terrainMap.get_packed_chunk(x,y,z)
        if chunk_str != '':
            self.client.send(self.add_prefix(3, chunk_str))
        else:
            print "send chunk error: chunk id invalid, " + str((x,y,z))

    @sendJSON
    def send_players(self):
        return {
            'cmd'    :  'player_list',
            'players':  GameStateGlobal.playerList.json()
        }

    @sendJSON
    def send_player(self, player):
        return {
            'cmd'   : 'player_info',
            'player': player.json(),
        }

    @sendJSON
    def remove_player(self, player):
        return {
            'cmd'   : 'remove_player',
            'id'    : player.id,
        }

    @sendJSON
    def remove_agent(self, agent):
        return {
            'cmd'   : 'remove_agent',
            'id'    : agent.id,
        }
