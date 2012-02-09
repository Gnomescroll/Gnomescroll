#!/usr/bin/python

import init_c_lib

'''
Client network outgoing
'''

class NetOut:
    sendPacket = None

    #mapMessage = None
    miscMessage = None

    @classmethod
    def init(cls):
        cls.sendPacket = NetClientGlobal.sendPacket
        #cls.mapMessage = MapMessage()
        cls.miscMessage = MiscMessage()
        
    @classmethod
    def send_json(cls, dict):
        cls.sendPacket.send_json(dict)

    @classmethod
    def send_binary(cls, msg_id, bin_string):
        cls.sendPacket.send_binary(msg_id, bin_string)

from net_client import NetClientGlobal

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
def idRequired(f):
    def wrapped(*args, **kwargs):
        if NetClientGlobal.connection.client_id:
            f(*args, **kwargs)
    return wrapped

""" Messages """
class GenericMessage:

    def __init__(self):
        pass

    def __call__(self, cmd, *args, **kwargs):
        self.__class__.__dict__[cmd](self, *args, **kwargs)


class MiscMessage:
    @sendJSON('ping')
    def ping(self):
        return { 'timestamp' : init_c_lib.get_ticks() }

'''
class MapMessage:

    @sendJSON('request_chunk_list')
    def request_chunk_list(self):
        return True

    @sendJSON('request_chunk')
    def request_chunk(self, x,y,z):
        return {
            'value' : (x,y,z),
        }
'''