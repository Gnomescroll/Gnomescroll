#!/usr/bin/python

import SDL.gl #for time stamp function

'''
Client network outgoing
'''

class NetOut:
    sendPacket = None

    sendMessage = None
    mapMessage = None
    adminMessage = None
    chatMessage = None
    miscMessage = None

    @classmethod
    def init_0(cls):
        cls.sendPacket = NetClientGlobal.sendPacket
        cls.sendMessage = SendMessage()
        cls.mapMessage = MapMessage()
        cls.adminMessage = AdminMessage()
        cls.chatMessage = ChatMessage()
        cls.miscMessage = MiscMessage()
    @classmethod
    def init_1(cls):
        assert cls.mapMessage != None
    @classmethod
    def send_json(cls, dict):
        cls.sendPacket.send_json(dict)
    @classmethod
    def send_binary(cls, msg_id, bin_string):
        cls.sendPacket.send_binary(msg_id, bin_string)

from net_client import NetClientGlobal
from game_state import GameStateGlobal


def sendJSON(cmd=None, tick=False):
    def outer(f, *args):
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

            if tick:
                json_data['tick'] = GameStateGlobal.gameState.time

            NetOut.send_json(json_data)
        return wrapped
    return outer

# if client_id is required
def idRequired(f):
    def wrapped(*args, **kwargs):
        if NetClientGlobal.client_id != '0':
            f(*args, **kwargs)
    return wrapped


class GenericMessage:

    def __init__(self):
        pass

    def __call__(self, cmd, *args, **kwargs):
        self.__class__.__dict__[cmd](self, *args, **kwargs)


class SendMessage(GenericMessage):

    @sendJSON('request_agent')
    def request_agent(self, agent_id):
        return {
            'aid'   :   agent_id,
        }

    @sendJSON('request_player')
    def request_player(self, player_id):
        return {
            'pid'   :   player_id,
        }

    @idRequired
    @sendJSON('agent_position', tick=True)
    def agent_position(self, agent):
        if agent is None or not agent.id:
            return
        return {
            'id'    :   agent.id,
            #'pos'   :   agent.pos(),
            'pos'   :   agent.state,
        }

    @idRequired
    @sendJSON('agent_control_state', tick=True)
    def send_agent_control_state(self, agent):
        if not agent.id :  # agent not identified
            return
        return {
            'state': agent.control_state(),
            'angle': [agent.x_angle, agent.y_angle],
            'aid'  : agent.id,
        }

    @idRequired
    @sendJSON('agent_button_state', tick=True)
    def agent_button_state(self, agent):
        return {
            'id'     : agent.id,
            'buttons' : agent.button_state,
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
            'weapon':   GameStateGlobal.agent.weapons.active().type
        }

    @idRequired
    @sendJSON('change_weapon', tick=True)
    def change_weapon(self, agent, active_weapon):
        if active_weapon is None:
            active_weapon = -1
        return {
            'aid'   :   agent.id,
            'windex':   active_weapon,
        }

    @idRequired
    @sendJSON('drop_weapon', tick=True)
    def drop_weapon(self, agent, wid):
        return {
            'aid'   :   agent.id,
            'wid'   :   wid,
        }

    @idRequired
    @sendJSON('pickup_item', tick=True)
    def pickup_item(self, agent, item, index=None):
        msg = {
            'aid'   :   agent.id,
            'iid'   :   item.id,
        }
        if index is not None:
            msg['slot'] = index
        return msg

    @idRequired
    @sendJSON('drop_item', tick=True)
    def drop_item(self, agent, item):
        return {
            'aid'   :   agent.id,
            'iid'   :   item.id,
        }

    @idRequired
    @sendJSON('hit_block', tick=True)
    def hit_block(self, agent=None):
        if agent is None or agent.id is None:
            return
        block_position = agent.nearest_block_position()
        if block_position is None:
            return
        return {
            'aid'   :   agent.id,
            'pos'   :   block_position,
        }

    @idRequired
    @sendJSON('set_block', tick=True)
    def set_block(self, agent=None):
        if agent is None or agent.id is None:
            return
        block_position = agent.facing_block_position()
        if block_position is None:
            return
        return {
            'aid'   :   agent.id,
            'type'  :   agent.active_block,
            'pos'   :   block_position,
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

    @idRequired
    @sendJSON('hitscan', tick=True)
    def hitscan(self, target):
        return {
            'target'    :   target
        }

    @idRequired
    @sendJSON('join_team')
    def join_team(self, team):
        return {
            'team'  :   team.id,
        }

class MiscMessage:
    @sendJSON('ping')
    def ping(self):
        return { 'timestamp' : SDL.gl.get_ticks() }

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
