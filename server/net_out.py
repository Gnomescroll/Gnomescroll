'''
Outgoing network messages
'''

try:
    import simplejson as json
except ImportError:
    import json
import struct

import dats.loader as dl

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
                if not client.admin:
                    client.send(event_packet)
        self.event_packets = []

    def add_json_event(self, dict):
        self.event_packets.append(SendMessage.get_json(dict))

    #Deprecate?
    @sendJSONevent('agent_update')
    def agent_update(self, agent, properties=None):
        print 'sending agent update'
        return {
            'agent' :   agent.json(properties),
        }

    #deprecate
    @sendJSONevent('projectile_create')
    def projectile_create(self, projectile):
        assert False
        return {
            'projectile'    :   projectile.json(),
        }

    #deprecate
    @sendJSONevent('projectile_destroy')
    def projectile_destroy(self, projectile):
        return {
            'id'    :   projectile.id,
        }

    #deprecate
    @sendJSONevent('projectile_update')
    def projectile_update(self, projectile):
        return {
            'projectile'    :   projectile.json(),
        }

    @sendJSONevent('item_update')
    def item_update(self, item, properties=None):
        return {
            'item'   :   item.json(properties),
        }

    @sendJSONevent('item_create')
    def item_create(self, item):
        return {
            'item'   :   item.json(),
        }

    @sendJSONevent('item_destroy')
    def item_destroy(self, item):
        return {
            'id'    :   item.id,
        }

    @sendJSONevent('team_update')
    def team_update(self, team, properties=None):
        return {
            'team'  :   team.json(properties),
        }

    @sendJSONevent('player_update', tick=False)
    def player_update(self, player, properties=None):
        return {
            'player': player.json(properties),
        }

    @sendJSONevent('weapon_update')
    def weapon_update(self, weapon, properties=None):
        print 'weapon update'
        return {
            'weapon'    :   weapon.json(properties)
        }

    @sendJSONevent('weapon_destroy')
    def weapon_destroy(self, weapon):
        return {
            'id'    :   weapon.id,
        }

    #deprecate
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

    @sendJSONevent('clear_map', tick=False)
    def clear_map(self):
        return True

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

    @sendJSONevent('team_list')
    def send_teams(self):
        return {
            'team_list' :   GameStateGlobal.teamList.json()
        }

    @sendJSONevent('player_list')
    def send_players(self):
        return {
            'player_list'   :   GameStateGlobal.playerList.json()
        }

    @sendJSONevent('dat')
    def send_dat(self, dat_name=None, type=None, key=None):
        if dat_name is None:
            d = dl.dat_loader.json()
        else:
            d = dl.dat_loader.get_json(dat_name, type, key)
        fin = {
            'dat'   :   d,
        }
        if dat_name is not None:
            fin['name'] = dat_name
            if type is not None:
                fin['type'] = type
                if key is not None:
                    fin['key'] = key
        return fin


#this is global message out across the connection pool
class MessageOut:
    def init(self):
        pass
    def __init__(self):
        pass

# calls send_json
#def sendJSON2(f):
    #def wrapped(*args):
        #self = args[0]
        #self.send_json(f(*args))
    #return wrapped

def sendJSON(cmd=None):
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

            self.send_json(json_data)
        return wrapped
    return outer

class SendMessage: #each connection has one of these
    #fmt = '<I H'
    fmt = '<H'
    @classmethod
    def add_prefix(cls, id, msg):
        return struct.pack(cls.fmt, id) + msg #length prefix not included in length?
    @classmethod
    def get_json(cls, dict):
        return cls.add_prefix(1, json.dumps(dict))
    def __init__(self, client):
        self.client = client
    def send_json(self, dict):
        jdump = json.dumps(dict)
        msg = self.add_prefix(1, jdump)
        self.client.send(msg)

    @sendJSON('dat')
    def send_dat(self, dat_name=None, type=None, key=None):
        if dat_name is None:
            d = dl.dat_loader.json()
        else:
            d = dl.dat_loader.get_json(dat_name, type, key)
        fin = {
            'dat'   :   d,
        }
        if dat_name is not None:
            fin['name'] = dat_name
            if type is not None:
                fin['type'] = type
                if key is not None:
                    fin['key'] = key
        return fin

    ## DEPRECATE
    @sendJSON('client_id')
    def send_client_id(self, connection):
        assert False
        print "Send client id"
        return {
            'id'   : connection.client_id,
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
            'full'  :   int(not properties),
        }

    #@sendJSON('agent_list')
    #def send_agents(self):
        #return {
            #'agent_list'    :   GameStateGlobal.agentList.json(),
        #}

    @sendJSON('agent_update')
    def send_agent(self, agent, properties=None):
        if type(agent) == int:
            agent = GameStateGlobal.agentList[agent]
        return {
            'agent' :   agent.json(properties),
            'full'  :   int(not properties),
        }

    @sendJSON('projectile_list')
    def send_projectiles(self):
        return {
            'projectile_list'   :   GameStateGlobal.projectileList.json(),
        }

    @sendJSON('projectile_update')
    def send_projectile(self, projectile, properties=None):
        if not hasattr(projectile, 'id'):
            projectile = GameStateGlobal.projectileList[projectile]
        return {
            'projectile'    :   projectile.json(properties),
            'full'  :   int(not properties),
        }

    @sendJSON('weapon_list')
    def send_weapons(self):
        return {
            'weapon_list'   :   GameStateGlobal.weaponList.json(),
        }

    @sendJSON('weapon_update')
    def send_weapon(self, weapon, properties=None):
        if not hasattr(weapon, 'id'):
            weapon = GameStateGlobal.weaponList[weapon]
        return {
            'weapon'    :   weapon.json(properties),
            'full'  :   int(not properties),
        }

    @sendJSON('item_list')
    def send_items(self):
        return {
            'item_list' :   GameStateGlobal.itemList.json(),
        }

    @sendJSON('item_update')
    def send_item(self, item, properties=None):
        if not hasattr(item, 'id'):
            item = GameStateGlobal.itemList[item]
        return {
            'item'  :   item.json(properties),
            'full'  :   int(not properties),
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
            'game'  :   GameStateGlobal.game.json(),
        }
        if send_teams:
            d['team_list'] = GameStateGlobal.teamList.json()
            d['teams'] = len(d['team_list'])
        return d

    @sendJSON('teams')
    def teams(self, teams):
        return {
            'team_list' :   [team.json() for team in teams],
        }

    @sendJSON('team_update')
    def send_team(self, team, properties=None):
        if not hasattr(team, 'id'):
            team = GameStateGlobal.teamList[team]
        d = {
            'team'  :   team.json(properties),
            'full'  :   int(not properties),
        }
        
