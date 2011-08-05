'''
Outgoing network messages
'''

import simplejson as json
import struct


class NetOut:

    event = None
    message = None

    @classmethod
    def init_0(cls):
        cls.event = EventOut()
        cls.message = MessageOut()

    @classmethod
    def init_1(cls):
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

    @sendJSONevent('agent_control_state')
    def agent_control_state(self, agent):
        return {
            'id'    :   agent.id,
            'state' :   agent.control_state(),
            'angle' :   agent.angle(),
        }

    @sendJSONevent('agent_button_state')
    def agent_button_state(self, agent):
        return {
            'id'        :   agent.id,
            'buttons'   :   agent.button_state,
        }

    @sendJSONevent('agent_update')
    def agent_update(self, agent, properties=None):
        print 'sending agent update'
        return {
            'agent' :   agent.json(properties),
        }

    # special case method for agent xyz. the client processes this differently than normal agent_update
    @sendJSONevent('agent_position')
    def agent_position(self, agent):
        return {
            'id'    :   agent.id,
            #'pos'   :   agent.pos(),
            'pos'   :   agent.state,
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

    @sendJSONevent('item_update')
    def item_update(self, obj, properties=None):
        return {
            'obj'   :   obj.json(properties),
        }

    @sendJSONevent('item_create')
    def item_create(self, obj):
        return {
            'obj'   :   obj.json(),
        }

    @sendJSONevent('item_destroy')
    def item_destroy(self, obj):
        return {
            'id'    :   obj.id,
        }

    @sendJSONevent('player_update', tick=False)
    def player_update(self, player, properties=None):
        return {
            'player': player.json(properties),
        }

    @sendJSONevent('player_create', tick=False)
    def player_create(self, player):
        return {
            'player'    :   player.json(),
        }

    @sendJSONevent('player_team', tick=False)
    def player_team(self, player):
        return {
            'id'    :   player.id,
            'team'  :   player.team.id,
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

    @sendJSONevent('hitscan')
    def hitscan(self, target, agent_id, weapon_type): # use projectile_id if we want multiple projectiles per weapon
        return {
            'target'    :   target,
            'aid'       :   agent_id,
            'wtype'     :   weapon_type,
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
    fmt = '<I H'
    @classmethod
    def add_prefix(cls, id, msg):
        return struct.pack(cls.fmt, 4+2+len(msg), id) + msg #length prefix not included in length?
    @classmethod
    def get_json(cls, dict):
        return cls.add_prefix(1, json.dumps(dict))
    def __init__(self, client):
        self.client = client
    def send_json(self, dict):
        jdump = json.dumps(dict)
        msg = self.add_prefix(1, jdump)
        self.client.send(msg)

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
            'list' : GameStateGlobal.terrainMap.get_server_chunk_list(),
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
            'player_list':  GameStateGlobal.playerList.json()
        }

    @sendJSON('player_update')
    def send_player(self, player, properties=None):
        if not hasattr(player, 'id'):
            player = GameStateGlobal.playerList[player]
        return {
            'player': player.json(properties),
        }

    @sendJSON('agent_list')
    def send_agents(self):
        return {
            'agent_list'    :   GameStateGlobal.agentList.json(),
        }

    @sendJSON('agent_update')
    def send_agent(self, agent, properties=None):
        if type(agent) == int:
            agent = GameStateGlobal.agentList[agent]
        return {
            'agent' :   agent.json(properties),
        }

    @sendJSON('projectile_list')
    def send_projectiles(self):
        return {
            'projectile_list'   :   GameStateGlobal.projectileList.json(),
        }

    @sendJSON('projectile_update')
    def send_projectile(self, projectile, properties=None):
        return {
            'projectile'    :   projectile.json(properties),
        }

    @sendJSON('weapon_list')
    def send_weapons(self):
        return {
            'weapon_list'   :   GameStateGlobal.weaponList.json(),
        }

    @sendJSON('weapon_update')
    def send_weapon(self, weapon, properties=None):
        return {
            'weapon'    :   weapon.json(properties),
        }

    @sendJSON('item_list')
    def send_items(self):
        return {
            'item_list' :   GameStateGlobal.itemList.json(),
        }

    @sendJSON('item_update')
    def send_item(self, item, properties=None):
        return {
            'item'  :   item.json(properties),
        }

    @sendJSON('player_destroy')
    def remove_player(self, player):
        return {
            'id'    : player.id,
        }

    @sendJSON('agent_destroy')
    def remove_agent(self, agent):
        return {
            'id'    : agent.id,
        }

    @sendJSON('you_died')
    def you_died(self, msg):
        return {
            'msg'   :   msg,
        }

    @sendJSON('you_killed')
    def you_killed(self, msg):
        return {
            'msg'   :   msg,
        }

    @sendJSON('ping')
    def ping(self, timestamp):
        return {
            'timestamp'   :   timestamp,
        }

    @sendJSON('game_mode')
    def game_mode(self, send_teams=True):
        d = {
            'mode'  :   GameStateGlobal.game_mode_name,
        }
        if send_teams:
            d['teams_list'] = GameStateGlobal.teamList.json()
            d['teams'] = len(d['teams_list'])
        return d

    @sendJSON('teams')
    def teams(self, teams):
        return {
            'teams_list' :   [team.json() for team in teams],
        }
        
