#!/usr/bin/python

'''
Client network outgoing
'''

import SDL.gl

class NetOut:
    sendPacket = None

    sendMessage = None
    adminMessage = None
    miscMessage = None
    
    @classmethod
    def init(cls):
        cls.sendPacket = NetClientGlobal.sendPacket
        cls.sendMessage = SendMessage()
        cls.adminMessage = AdminMessage()
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

    @sendJSON('admin')
    def is_admin(self):
        return True


class MiscMessage:
    @sendJSON('ping')
    def ping(self):
        return { 'timestamp' : SDL.gl.get_ticks() }


class AdminMessage:

    @sendJSON('set_map')
    def set_map(self,x,y,z,value):
        print "set map"
        return {
            'list' : [(x,y,z,value)],
        }

    @sendJSON('set_map')
    def set_map_bulk(self, l): #takes a list of 4 tuples of (x,y,z,value)
        print 'sending bulk'
        return {
            'list' : l,
        }

    @sendJSON('clear_map')
    def clear_map(self):
        return True
