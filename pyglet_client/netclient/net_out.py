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


def sendJSON(cmd=None, tick=False):
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

            if tick:
                json_data['tick'] = GameStateGlobal.gameState.time

            NetOut.send_json(json_data)
        return wrapped
    return outer

# if client_id is required
def idRequired(f):
    def wrapped(*args):
        if NetClientGlobal.client_id != '0':
            f(*args)
    return wrapped


class GenericMessage:

    def __init__(self):
        pass

    def __call__(self, cmd, *args, **kwargs):
        self.__class__.__dict__[cmd](self, *args, **kwargs)

    
class SendMessage(GenericMessage):

    @idRequired
    @sendJSON('agent_control_state', tick=True)
    def send_agent_control_state(self, agent):
        if not agent.id :  # agent not identified
            return
        return {
            'state': agent.control_state,
            'angle': [agent.x_angle, agent.y_angle],
            'aid'  : agent.id,
        }

    @idRequired
    @sendJSON('fire_projectile', tick=True)
    def fire_projectile(self, agent=None):
        if agent is None or agent.id is None:
            return
        return {
            'aid'   : agent.id,
        }

    @idRequired
    @sendJSON('reload_weapon', tick=True)
    def reload_weapon(self, agent=None):
        if agent is None or agent.id is None:
            return
        return {
            'aid'   :   agent.id,
            'weapon':   GameStateGlobal.agent.active_weapon.key()
        }

    @idRequired
    @sendJSON('hit_block', tick=True)
    def hit_block(self, agent=None):
        if agent is None or agent.id is None:
            return
        return {
            'aid'   :   agent.id,
        }

    @idRequired
    @sendJSON('set_block', tick=True)
    def set_block(self, agent=None):
        if agent is None or agent.id is None:
            return
        return {
            'aid'   :   agent.id,
            'type'  :   agent.active_block,
        }

    @sendJSON('identify')
    def identify(self, name=None):
        if name is None:
            name = NetClientGlobal.name
        return {
            'name': name,
        }

    @sendJSON('request_client_id')
    def request_client_id(self):
        pass

class MapMessage:

    @sendJSON('request_chunk_list')
    def request_chunk_list(self):
        pass

    @sendJSON('request_chunk')
    def request_chunk(self, x,y,z):
        return {
            'value' : (x,y,z),
        }

class ChatMessage:

    @idRequired
    @sendJSON('chat')
    def send_chat(self, d):
        d['cid'] = str(NetClientGlobal.client_id)
        return d

    @idRequired
    @sendJSON('subscribe')
    def subscribe(self, channel):
        return {
            'channel'   : channel,
            'cid' : str(NetClientGlobal.client_id),
        }

    @idRequired
    @sendJSON('unsubscribe')
    def unsubscribe(self, channel):
        return {
            'channel'   : channel,
            'cid' : str(NetClientGlobal.client_id),
        }

class AdminMessage:

    @sendJSON('set_map')
    def set_map(self,x,y,z,value):
        print "set map"
        return {
            'list' : [(x,y,z,value)],
        }

    @sendJSON('set_map_bulk')
    def set_map_bulk(self, list): #takes a list of 4 tuples of (x,y,z,value)
        return {
            'list' : list,
        }
