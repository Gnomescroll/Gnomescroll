import socket
import struct
#import binascii

from world_state import WorldStateGlobal
from net_event import NetEventGlobal
from net_api import NetApiGlobal

import simplejson as json

class NetClientGlobal:
    connection = None
    sendMessage = None

    client_id = 0
    VERSION = '0.0.1'

    def __init__(self):
        NetClientGlobal.connection = TcpConnection()

    @classmethod
    def init(self):
        self.clientDatagramDecoder.init()
        assert self.connection != None
        assert self.sendMessage != None

    @classmethod
    def connect(self):
        self.connection.connect()

#import binascii

class SendMessage:
    def __init__(self, client):
        self.client = client
        NetClientGlobal.sendMessage = self
    def add_prefix(self,id, msg):
        return struct.pack('I H', 4+2+len(msg), id) + msg #length prefix is included in length
    def send_json(self, dict):
        self.client.send(self.add_prefix(1, json.dumps(dict)))  #fix this
    def send_binary(self,msg_id, bin_string):
        self.client.send(self.add_prefix(msg_id, bin_string))

class ClientDatagramDecoder:

    messageHandler = None
    def init(self):
        self.messageHandler = NetEventGlobal.messageHandler
        assert self.messageHandler != None
    def __init__(self, connection):
        self.connection = connection

    def process_datagram(self, message):
        (prefix, datagram) = (message[0:6],message[6:])
        (length, msg_type) = struct.unpack('I H', prefix)
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
            length = self.message_length
            self.process_msg(message)
            self.message_length = 0
            self.attempt_decode()

    def read_prefix(self):
        data = self.buffer
        prefix = data[0:4]
        (length,) = struct.unpack('I', data[0:4])
        return length

    def process_msg(self, message):
        self.count += 1
        if self.count % 500 == 0:
            print "processed message count: " +str(self.count)
        self.datagramDecoder.process_datagram(message)

import select

class TcpConnection:
    server = '127.0.0.1'
    tcp_port = 5055
    #settings
    noDelay = True

    def __init__(self):
        self.tcp = None
        self.connected = False
        self.out = SendMessage(self)
        self.decoder = PacketDecoder(self)

        self.fileno = 0
        self._epoll = select.epoll()

        self.ec = 0

        NetClientGlobal.connection = self
        #self.connect()

    def connect(self):
        TCP_IP = self.server
        TCP_PORT = self.tcp_port
        try:
            self.decoder.reset()
            self.tcp = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.tcp.connect((TCP_IP, TCP_PORT))
            if self.noDelay == True:
                self.tcp.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
            self.tcp.setblocking(0) #should be blocking?
            self._epoll.register(self.tcp.fileno(), select.EPOLLIN | select.EPOLLHUP | select.EPOLLOUT )
            print "Connection: tcp connected"
            self.fileno = self.tcp.fileno()
            self.connected = True
            #on connect
            if NetClientGlobal.client_id != 0: #reconnection case
                self.out.send_client_id()
        except socket.error, (value,message):
            print "Connection failed: socket error " + str(value) + ", " + message
            self.connected = False

    def close(self):
        print "Connection: tcp disconnected by program"
        self.connected = False
        self._epoll.unregister(self.fileno)
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
        events = self._epoll.poll(0)
        for fileno, event in events:
            assert fileno == self.fileno
            if event & select.EPOLLIN:
                #print "Read Event"
                self.recv()
            elif event & select.EPOLLOUT:
                pass #ready to write
            else:
                print "Strange Epoll Event: %i" % eventOut

    def recv(self):
        BUFFER_SIZE = 4096
        try:
            data = self.tcp.recv(BUFFER_SIZE)
        except socket.error, (value,message):
            print "TcpClient.get: socket error %i, %s" % (value, message)
            data = ''
        if len(data) == 0: #if we read three times and get no data, close socket
            #print "tcp data: empty read"
            self.ec += 1
            if self.ec > 3:
                self.disconnect()
        else:
            #print "get_tcp: data received, %i bytes" % len(data)
            self.ec = 0
            self.decoder.add_to_buffer(data)
