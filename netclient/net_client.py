#!/usr/bin/python

'''
Client network interface
'''

import opts
opts = opts.opts

#import socket
import struct
import json
import select

_epoll = 0

class NetClientGlobal:
    connection = None
    sendPacket = None

    client_id = '0'
    name = opts.name
    VERSION = opts.version

    @classmethod
    def init_0(cls):
        pass
        #NetClientGlobal.connection = TcpConnection()
        NetClientGlobal.sendPacket = SendPacket()
    @classmethod
    def init_1(cls):
        #ClientDatagramDecoder.init()
        #assert cls.connection != None
        assert cls.sendPacket != None

    @classmethod
    def connect(cls):
        return
        #cls.connection.connect()

import init_c_lib
from init_c_lib import get_client_id, connected, _send_python_net_message

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

'''
class ClientDatagramDecoder:

    messageHandler = None
    @classmethod
    def init(cls):
        cls.messageHandler = NetEventGlobal.messageHandler
        assert cls.messageHandler != None
    def __init__(self, connection):
        self.connection = connection
        self.fmt = '<I H'
        self.fmtlen = struct.calcsize(self.fmt)

    def process_datagram(self, message):
        prefix, datagram = message[:self.fmtlen], message[self.fmtlen:]
        length, msg_type = struct.unpack(self.fmt, prefix)
        self.messageHandler.process_net_event(msg_type, datagram)
'''

'''
    def process_datagram(self, message):
        prefix, datagram = message[:self.fmtlen], message[self.fmtlen:]
        length, msg_type = struct.unpack(self.fmt, prefix)
        self.messageHandler.process_net_event(msg_type, datagram)
'''

class TcpConnection:
    #server = opts.server
    #tcp_port = opts.tcp_port
    #noDelay = True

    def __init__(self):
        #global _epoll
        #self.tcp = None
        self.connected = False
        self.out = SendPacket(self)
        ##self.decoder = PacketDecoder(self)

    def connect(self):
        return
    def close(self):
        pass
    def send(self, MESSAGE):
        assert False
    def attempt_recv(self):
        return
    def recv(self):
        assert False

from game_state import GameStateGlobal
from net_event import NetEventGlobal
from net_out import NetOut
