#!/usr/bin/python

'''
Client network interface
'''

import opts
opts = opts.opts

import struct
import json
import select

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

    @classmethod
    def client_id_from_name(cls, name):
        for agent in GameStateGlobal.agentList.values():
            if agent.name == name:
                return agent.client_id

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
        #return struct.pack(self.fmt, 4+2+len(msg), id) + msg #length prefix is included in length
    def send_json(self, dict):
        _send_python_net_message( self.add_prefix(1, json.dumps(dict)) , 0)
        #self.client.send(self.add_prefix(1, json.dumps(dict)))
    def send_binary(self,msg_id, bin_string):
        _send_python_net_message( self.add_prefix(msg_id, bin_string), 0 )
        #self.client.send(self.add_prefix(msg_id, bin_string))

_msg_buffer = True

class PyClient(object):

    messageHandler = None
    #@classmethod
    #def init(cls):
        #cls.messageHandler = NetEventGlobal.messageHandler
        #assert cls.messageHandler != None
    def __init__(self):
        self.connected = False
        self.out = SendPacket(self)

        self.fmt = '<H'
        self.fmtlen = struct.calcsize(self.fmt)
        #PyClient.messageHandler = NetEventGlobal.messageHandler
        #assert PyClient.messageHandler != None

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
        # HACK -- This is required or else python will not accept dats
        # Could not figure out why.
        NetOut.mapMessage.request_chunk(0,0,0)
        NetOut.sendMessage.identify()

    def on_disconnect(self):
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
        #self.messageHandler.process_net_event(msg_type, message)
        NetEventGlobal.messageHandler.process_net_event(msg_type, message)

    def close(self):
        pass

from game_state import GameStateGlobal
from net_event import NetEventGlobal
from net_out import NetOut
