#!/usr/bin/python

'''
Client network interface
'''

import socket
import struct

import json

import os
OS = os.uname()[0]
#OS = "Windows"

import opts

if OS == "Linux":
    pass
elif OS == "Windows":
    pass
elif OS == "Darwin":
    pass

import select
_epoll = 0

class NetClientGlobal:
    connection = None
    sendPacket = None

    client_id = '0'
    VERSION = opts.version

    @classmethod
    def init(cls):
        NetClientGlobal.connection = TcpConnection()
    @classmethod
    def init1(cls):
        ClientDatagramDecoder.init()
        assert cls.connection != None
        assert cls.sendPacket != None

    @classmethod
    def connect(cls):
        cls.connection.connect()

class SendPacket:
    def __init__(self, client):
        self.client = client
        self.fmt = '<I H'
        NetClientGlobal.sendPacket = self
    def add_prefix(self,id, msg):
        return struct.pack(self.fmt, 4+2+len(msg), id) + msg #length prefix is included in length
    def send_json(self, dict):
        self.client.send(self.add_prefix(1, json.dumps(dict)))  #fix this
    def send_binary(self,msg_id, bin_string):
        self.client.send(self.add_prefix(msg_id, bin_string))

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

class PacketDecoder:
    def __init__(self,connection):
        self.datagramDecoder = ClientDatagramDecoder(connection)
        self.reset()

    def reset(self):
        self.buffer = ''
        self.message_length = 0
        self.count = 0

    def add_to_buffer(self,data):
        self.buffer += data
        self.attempt_decode()

    def attempt_decode(self):
        buff_len = len(self.buffer)
        if buff_len == 0:
            return
        elif self.message_length == 0 and buff_len > 6:
            self.message_length = self.read_prefix()
        elif buff_len < self.message_length:
            return
        elif self.message_length == 0:
            return

        if buff_len >= self.message_length:
            assert self.message_length > 0
            (message, self.buffer) = (self.buffer[:self.message_length], self.buffer[self.message_length:])
            #length = self.message_length
            self.process_msg(message)
            self.message_length = 0
            self.attempt_decode()

    def read_prefix(self):
        data = self.buffer
        #prefix = data[0:4]
        fmt = '<I'
        fmtlen = struct.calcsize(fmt)
        (length,) = struct.unpack(fmt, data[0:fmtlen])
        return length

    def process_msg(self, message):
        self.count += 1
        if self.count % 500 == 0:
            print "processed message count: " +str(self.count)
        self.datagramDecoder.process_datagram(message)

class TcpConnection:
    server = opts.server
    tcp_port = opts.port
    #settings
    noDelay = True

    def __init__(self):
        global _epoll
        self.tcp = None
        self.connected = False
        self.out = SendPacket(self)
        self.decoder = PacketDecoder(self)

        self.fileno = 0
        if _epoll == 1:
            self._epoll = select.epoll()
        else:
            self.socket = 0
        self.ec = 0

        NetClientGlobal.connection = self

    def connect(self):
        global _epoll
        TCP_IP = self.server
        TCP_PORT = self.tcp_port
        try:
            self.decoder.reset()
            self.tcp = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.tcp.connect((TCP_IP, TCP_PORT))
            if self.noDelay == True:
                self.tcp.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
            self.tcp.setblocking(0) #should be blocking?
            if _epoll == 1:
                self._epoll.register(self.tcp.fileno(), select.EPOLLIN | select.EPOLLHUP | select.EPOLLOUT )
            else:
                self.socket = self.tcp.fileno()
            print "Connection: tcp connected"
            self.fileno = self.tcp.fileno()
            self.connected = True
        except socket.error, (value,message):
            print "Connection failed: socket error " + str(value) + ", " + message
            self.connected = False

    def close(self):
        global _epoll
        print "Connection: tcp disconnected by program"
        self.connected = False
        if _epoll == 1:
            self._epoll.unregister(self.fileno)
        else:
            self.socket = 0
        self.tcp.close()

    def send(self, MESSAGE):
        #print "send len: " + str(len(MESSAGE))
        if self.connected == True:
            try:
                self.tcp.sendall(MESSAGE)
            except socket.error, (value,message):
                print "Connection failed: socket error " + str(value) + ", " + message
                if value == 32:  #connection reset by peer
                    self.close()
        else:
            print "TcpConnection.send: Socket is not connected!"

    def attempt_recv(self):
        global _epoll
        if _epoll == 1:
            events = self._epoll.poll(0)
            for fileno, event in events:
                assert fileno == self.fileno
                if event & select.EPOLLIN:
                    #print "Read Event"
                    self.recv()
                elif event & select.EPOLLOUT:
                    pass #ready to write
                else:
                    print "Strange Epoll Event: %i" % event
        else:
            socket = self.socket
            if socket == 0:
                #print "Socket not connected"
                return
            rlist, wlist, xlist = select.select([socket], [], [], 0.0)
            #print "rlist= " + str(rlist)
            if socket in rlist:
                self.recv()
    def recv(self):
        BUFFER_SIZE = 4096
        try:
            data = self.tcp.recv(BUFFER_SIZE)
            #print "data= " + str(data)
        except socket.error, (value,message):
            print "TcpClient.get: socket error %i, %s" % (value, message)
            data = ''
        if len(data) == 0: #if we read three times and get no data, close socket
            #print "tcp data: empty read"
            self.ec += 1
            if self.ec > 3:
                self.close()
        else:
            #print "get_tcp: data received, %i bytes" % len(data)
            self.ec = 0
            self.decoder.add_to_buffer(data)

from net_event import NetEventGlobal
