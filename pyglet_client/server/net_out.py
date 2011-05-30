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


def sendJSONevent(cmd=None, tick=True):
    def outer(f, *args):
        def wrapped(*args):
            self = args[0]
            json_data = f(*args)
            if json_data is None:
                json_data = {}

            cmd_final = cmd # must do this reassignment due to function scoping
            if cmd_final is None:
                cmd_final = ''
            if cmd_final or 'cmd' not in json_data:
                json_data['cmd'] = cmd_final

            if tick:
                json_data['tick'] = GameStateGlobal.gameState.time

            self.add_json_event(json_data)
        return wrapped
    return outer

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

    @sendJSONevent('agent_position')
    def agent_state_change(self, agent):
        return {
            'id'   : agent.id,
            'state': agent.state, #is a 9 tuple
        }

    @sendJSONevent('agent_update')
    def agent_update(self, agent, properties=None):
        return {
            'agent' :   agent.json(properties),
        }

    @sendJSONevent('projectile_create')
    def projectile_create(self, projectile):
        return {
            'projectile'    :   projectile.json(),
        }

    @sendJSONevent('projectile_destroy')
    def projectile_destroy(self, projectile):
        return {
            'id'    :   projectile.id,
        }

    @sendJSONevent('projectile_update')
    def projectile_update(self, projectile):
        return {
            'projectile'    :   projectile.json(),
        }

    @sendJSONevent('player_update', tick=False)
    def player_update(self, player):
        return {
            'player': player.json(),
        }

    @sendJSONevent('player_update', tick=False)
    def player_rename(self, player):
        return {
            'cmd'   : 'player_update',
            'player': {
                'name'  : player.name,
                'id'    : player.id,
            },
        }

    @sendJSONevent('player_info', tick=False)
    def player_join(self, player):
        return {
            'player': player.json(),
        }

    @sendJSONevent('client_quit', tick=False)
    def client_quit(self, client_id):
        return {
            'id'    : client_id,
        }

    @sendJSONevent('set_map', tick=False)
    def set_map(self, list):
        return {
            'list' : list,
        }

#this is global message out across the connection pool
class MessageOut:
    def init(self):
        pass
    def __init__(self):
        pass

# calls send_json
def sendJSON2(f):
    def wrapped(*args):
        self = args[0]
        self.send_json(f(*args))
    return wrapped

def sendJSON(cmd=None):
    def outer(f, *args):
        def wrapped(*args):
            self = args[0]
            json_data = f(*args)
            if json_data is None:
                json_data = {}

            cmd_final = cmd # must do this reassignment due to function scoping
            if cmd_final is None:
                cmd_final = ''
            if cmd_final != '' or 'cmd' not in json_data:
                json_data['cmd'] = cmd_final

            self.send_json(json_data)
        return wrapped
    return outer

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
    @sendJSON('client_id')
    def send_client_id(self, connection):
        print "Send client id"
        return {
            'id'   : connection.id,
        }

    @sendJSON('client_quit')
    def client_quit(self, client_id):
        return {
            'id'    : client_id,
        }

    @sendJSON('identify_fail')
    def identify_fail(self, connection, notes=''):
        return {
            'msg'   : notes,
        }

    @sendJSON('identified')
    def identified(self, connection, msg=''):
        print 'Identified'
        return {
            'msg'   : msg,
            'player': connection.player.json(),
        }

    @sendJSON('chunk_list')
    def send_chunk_list(self):
        return {
            'list' : GameStateGlobal.terrainMap.get_chunk_list(),
        }

    def send_chunk(self, x, y, z):
        chunk_str = GameStateGlobal.terrainMap.get_packed_chunk(x,y,z)
        if chunk_str != '':
            self.client.send(self.add_prefix(3, chunk_str))
        else:
            print "send chunk error: chunk id invalid, " + str((x,y,z))

    @sendJSON('player_list')
    def send_players(self):
        return {
            'players':  GameStateGlobal.playerList.json()
        }

    @sendJSON('player_info')
    def send_player(self, player):
        return {
            'player': player.json(),
        }

    @sendJSON('remove_player')
    def remove_player(self, player):
        return {
            'id'    : player.id,
        }

    @sendJSON('remove_agent')
    def remove_agent(self, agent):
        return {
            'id'    : agent.id,
        }
