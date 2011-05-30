#!/usr/bin/python

'''
Client network outgoing
'''

class NetOut:
    sendPacket = None

    sendMessage = None
    mapMessage = None
    adminMessage = None
    chatMessage = None

    @classmethod
    def init_0(self):
        NetOut.sendPacket = NetClientGlobal.sendPacket
        NetOut.sendMessage = SendMessage()
        NetOut.mapMessage = MapMessage()
        NetOut.adminMessage = AdminMessage()
        NetOut.chatMessage = ChatMessage()
    @classmethod
    def init_1(self):
        assert NetOut.mapMessage != None
    @classmethod
    def send_json(self, dict):
        self.sendPacket.send_json(dict)
    @classmethod
    def send_binary(self, msg_id, bin_sting):
        self.sendPacket.send_binary(msg_id, bin_string)

from net_client import NetClientGlobal
from game_state import GameStateGlobal

# calls send_json
def sendJSON(f):
    def wrapped(*args):
        package = f(*args)
        if package is not None:
            NetOut.send_json(package)
    return wrapped

# if client_id is required
def idRequired(f):
    def wrapped(*args):
        if NetClientGlobal.client_id != '0':
            f(*args)
    return wrapped
    
class SendMessage:

    @idRequired
    @sendJSON
    def send_agent_control_state(self, agent_id, d_x, d_y, v_x, v_y, jetpack, jump, brake):
        if agent_id is None:  # agent not identified
            return
        return {
            'cmd' : 'agent_control_state',
            'tick' : 0,
            'state': [d_x, d_y, v_x, v_y, jetpack, jump, brake],
            'aid'  : agent_id,
           }

    @idRequired
    @sendJSON
    def fire_projectile(self, agent_id=None):
        if agent_id is None:
            agent_id = GameStateGlobal.agent.id
        if agent_id is None:
            return
        return {
            'cmd'   : 'fire_projectile',
            'tick'  : 0,
            'aid'   : agent_id,
        }

    @sendJSON
    def identify(self, name=None):
        if name is None:
            name = NetClientGlobal.name
        return {
            'cmd': 'identify',
            'name': name,
        }

    @sendJSON
    def request_client_id(self):
        return {
            'cmd'   : 'request_client_id',
        }

class MapMessage:

    @sendJSON
    def request_chunk_list(self):
        return {
            'cmd' : 'request_chunk_list',
        }

    @sendJSON
    def request_chunk(self, x,y,z):
        return {
            'cmd' : 'request_chunk',
            'value' : (x,y,z)
        }

class ChatMessage:

    @sendJSON
    def send_chat(self, d):
        if NetClientGlobal.client_id == 0:
            return
        d['cmd'] = 'chat'
        d['cid'] = str(NetClientGlobal.client_id)
        return d

    @sendJSON
    def subscribe(self, channel):
        if NetClientGlobal.client_id == 0:
            print 'client_id is 0, abort'
            return
        return {
            'channel'   : channel,
            'cmd'       : 'subscribe',
            'cid' : str(NetClientGlobal.client_id),
        }

    @sendJSON
    def unsubscribe(self, channel):
        if NetClientGlobal.client_id == 0:
            return
        return {
            'channel'   : channel,
            'cmd'       : 'unsubscribe',
            'cid' : str(NetClientGlobal.client_id),
        }

class AdminMessage:

    @sendJSON
    def set_map(self,x,y,z,value):
        return {
            'set_map' : 'set_map',
            'list' : [(x,y,z,value)],
            }

    @sendJSON
    def set_map_bulk(self, list): #takes a list of 4 tuples of (x,y,z,value)
        return {
            'set_map' : 'set_map',
            'list' : list,
            }
