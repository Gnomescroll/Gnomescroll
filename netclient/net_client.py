#!/usr/bin/python

'''
Client network interface
'''

import opts
opts = opts.opts

import struct
import json

_epoll = 0

class NetClientGlobal:
    connection = None
    sendPacket = None

    name = opts.name
    VERSION = opts.version

    @classmethod
    def init(cls):
        NetClientGlobal.connection = PyClient()
        NetClientGlobal.sendPacket = SendPacket()

import init_c_lib
from init_c_lib import get_client_id, connected, _send_python_net_message
from init_c_lib import register_client_creation, register_client_deletion, register_client_message_handling

class SendPacket:
    def __init__(self, client=0):
        NetClientGlobal.sendPacket = self
        self.fmt = '<H'
        self.fmtlen = struct.calcsize(self.fmt)
    def add_prefix(self,id, msg):
        return struct.pack(self.fmt,id) + msg
    def send_json(self, dict):
        _send_python_net_message( self.add_prefix(1, json.dumps(dict)) , 0)
    def send_binary(self,msg_id, bin_string):
        _send_python_net_message( self.add_prefix(msg_id, bin_string), 0 )

_msg_buffer = True

class PyClient(object):

    messageHandler = None

    def __init__(self):
        self.connected = False
        self.out = SendPacket(self)

        self.fmt = '<H'
        self.fmtlen = struct.calcsize(self.fmt)

        global _msg_buffer     
        if _msg_buffer: 
            self.message_buffer = []
            register_client_message_handling(( lambda _client_id, message : self.push_to_buffer(message) ))

        register_client_creation(( lambda client_id: self.on_connect(client_id) ))
        register_client_deletion(( lambda client_id: self.on_disconnect(client_id) ))

    def __getattribute__(self, k):
        if k == 'client_id':
            return get_client_id()
        return object.__getattribute__(self, k)

    def on_connect(self, client_id):
        print "NetClient connected: client id = %i" % (client_id)
        self.connected = True
        init_c_lib.identify(NetClientGlobal.name)

    def on_disconnect(self, client_id):
        print "NetClient disconnected"
        self.connected = False

    def push_to_buffer(self, message):
        self.message_buffer.append(message)

    def dispatch_buffer(self):
        global _msg_buffer     
        if not _msg_buffer:
            return
        for message in self.message_buffer:
            self.handleMessage(message)
        self.message_buffer = []

    def handleMessage(self, message):
        msg_type = struct.unpack(self.fmt, message[0:2])
        msg_type = msg_type[0]
        message = message[2:]
        NetEventGlobal.messageHandler.process_net_event(msg_type, message)

    def close(self):
        pass

from net_event import NetEventGlobal
from net_out import NetOut
