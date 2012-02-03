#!/usr/bin/python

import init_c_lib

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
    datMessage = None

    @classmethod
    def init(cls):
        cls.sendPacket = NetClientGlobal.sendPacket
        cls.sendMessage = SendMessage()
        cls.mapMessage = MapMessage()
        cls.adminMessage = AdminMessage()
        cls.chatMessage = ChatMessage()
        cls.miscMessage = MiscMessage()
        cls.datMessage = DatMessage()
        
    @classmethod
    def send_json(cls, dict):
        cls.sendPacket.send_json(dict)

    @classmethod
    def send_binary(cls, msg_id, bin_string):
        cls.sendPacket.send_binary(msg_id, bin_string)

from net_client import NetClientGlobal
from game_state import GameStateGlobal

def sendJSON(cmd=None):
    def outer(f, *args):
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
            if cmd_final != '' or 'cmd' not in json_data:
                json_data['cmd'] = cmd_final

            NetOut.send_json(json_data)
        return wrapped
    return outer

""" Decorators """
# if client_id is required
def idRequired(f):
    def wrapped(*args, **kwargs):
        if NetClientGlobal.connection.client_id:
            f(*args, **kwargs)
    return wrapped

def noViewer(f):
    def wrapped(*args, **kwargs):
        if not GameStateGlobal.agent.is_viewer():
            f(*args, **kwargs)
    return wrapped

""" Messages """
class GenericMessage:

    def __init__(self):
        pass

    def __call__(self, cmd, *args, **kwargs):
        self.__class__.__dict__[cmd](self, *args, **kwargs)


class SendMessage(GenericMessage):

    @idRequired
    @sendJSON('received_client_id')
    def received_client_id(self):
        return True
    @sendJSON('identify')
    def identify(self, name=None):
        if name is None:
            name = NetClientGlobal.name
        return {
            'name': name,
        }

class MiscMessage:
    @sendJSON('ping')
    def ping(self):
        return { 'timestamp' : init_c_lib.get_ticks() }

class DatMessage:
    @sendJSON('dat_loaded')
    def loaded(self):
        return True

class MapMessage:

    @sendJSON('request_chunk_list')
    def request_chunk_list(self):
        return True

    @sendJSON('request_chunk')
    def request_chunk(self, x,y,z):
        return {
            'value' : (x,y,z),
        }

class ChatMessage:

    @idRequired
    @sendJSON('chat')
    def send_chat(self, d):
        d['cid'] = str(NetClientGlobal.connection.client_id)
        return d

    @idRequired
    @sendJSON('subscribe')
    def subscribe(self, channel):
        return {
            'channel'   : channel,
            'cid' : str(NetClientGlobal.connection.client_id),
        }

    @idRequired
    @sendJSON('unsubscribe')
    def unsubscribe(self, channel):
        return {
            'channel'   : channel,
            'cid' : str(NetClientGlobal.connection.client_id),
        }

class AdminMessage:

    @sendJSON('set_map')
    def set_map(self,x,y,z,value):
        return {
            'list' : [(x,y,z,value)],
        }

    @sendJSON('set_map_bulk')
    def set_map_bulk(self, list): #takes a list of 4 tuples of (x,y,z,value)
        return {
            'list' : list,
        }
