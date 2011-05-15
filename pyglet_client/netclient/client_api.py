import socket
import struct
import binascii

from collections import namedtuple

import simplejson as json

class ClientGlobal:
    connection = None
    sendMessage = None
    sendAdminMessage = None
    messageHandler = None
    client_id = 0

    def __init__(self):
        pass

    @classmethod
    def init(self):
        pass

class SendMessage:
    def __init__(self, client):
        self.client = client
        ClientGlobal.sendMessage = self
    def add_prefix(self, id, msg):
        return struct.pack('I H', 4+2+len(msg), id) + msg #length prefix is included in length
    def send_json(self, dict):
        self.client.send(self.add_prefix(1, json.dumps(dict)))
    def get_json(self, dict):
        return self.add_prefix(1, json.dumps(dict))
    ### agent messages

    def send_agent_control_state(self, id, d_x, d_y, d_xa, d_za, jetpack, brake):
        d = {
            'cmd' : 'agent_control_state',
            'id' : id,
            'tick' : 0,
            'state': [d_x, d_y, d_xa, d_za, jetpack, brake]
           }
        self.send_json(d)

    def send_chat(self, d):
        d['cmd'] = 'chat'
        self.send_json(d)

    def send_client_id(self):
        d = {
            'cmd' : 'send_client_id',
            'id' : ClientGlobal.client_id,
           }
        self.send_json(d)

    def request_chunk_list(self):
        d = {
            'cmd' : 'request_chunk_list',
            }
        self.send_json(d)

    def request_chunk(x,y,z):
        d = {
            'cmd' : 'request_chunk',
            'value' : (x,y,z)
        }

class SendAdminMessage:
    def __init__(self, client):
        self.client = client
        ClientGlobal.sendAdminMessage = self
    def add_prefix(self, id, msg):
        return struct.pack('I H', 4+2+len(msg), id) + msg #length prefix is included in length
    def send_json(self, dict):
        self.client.send(self.add_prefix(2, json.dumps(dict)))
    def get_json(self, dict):
        return self.add_prefix(1, json.dumps(dict))
## Admin messages

    def set_map(self,x,y,z,value):
        d = {
            'set_map' : 'set_map',
            'list' : [(x,y,z,value)],
            }
        self.send_json(d)

    def set_map_bulk(self, list): #takes a list of 4 tuples of (x,y,z,value)
        d = {
            'set_map' : 'set_map',
            'list' : list,
            }
        self.send_json(d)

class MessageHandler:
    def __init__(self, player):
        self.player = player
        ClientGlobal.messageHandler = self

    def process_json(self, msg):
        cmd = msg.get('cmd', None)
        if cmd == 'agent_position':
            self._agent_position(**msg)
        elif cmd == 'send_client_id':
            self._set_client_id(**msg)
        elif cmd == 'chunk_list':
            print "Chunk List Received"
            print str(msg['list'])
        else:
            print "JSON message type unregonized"

    def _agent_position(self, id, tick, state, **misc):
        [x,y,z,vx, vy, vz,ax, ay, az] = state
        [x,y,z] = [float(x),float(y),float(z)]

        self.player.x = x
        self.player.y = y
        self.player.z = z
        self.player.vx = vx
        self.player.vy = vy
        self.player.vz = vz
        self.player.ax = ax
        self.player.ay = ay
        self.player.az = az

    def _set_client_id(self, id, **misc):
        print "Received Client Id: %i" % (id,)
        if ClientGlobal.client_id == 0:
            ClientGlobal.client_id = id
            ClientGlobal.sendMessage.send_client_id()


import binascii

class ClientDatagramDecoder:

    messageHandler = None

    def __init__(self, connection):
        assert self.messageHandler != None
        self.connection = connection

    def process_datagram(self, message):
        #print "decoding datagram"
        (prefix, datagram) = (message[0:6],message[6:])
        (length, msg_type) = struct.unpack('I H', prefix)

        if msg_type == 0:
            print "test message received"
        elif msg_type == 1: #json
            self._1_json(msg)
        elif msg_type == 3:
            self._3_map_chunk(msg)
        else:
            print "unknown message type: %i" % msg_type

    def _1_json(self, msg):
        try:
            #print "json message"
            msg = json.loads(datagram)
        except:
            print "error decoding: len = %i, message_length= %i" % (len(datagram), length)
            msg = { 'cmd' : 'error' }
        self.messageHandler.process_json(msg)

    def _3_map_chunk(self, msg):
        print "Map Chunk Received"

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
            #print "decode: buffer empty"
            return
        elif self.message_length == 0 and buff_len > 6:
            #print "decode: get message prefix"
            self.message_length = self.read_prefix()
            #print "prefix length: " + str(self.message_length)
        elif buff_len < self.message_length:
            return
        elif self.message_length == 0:
            return

        if buff_len >= self.message_length:
            assert self.message_length > 0
            #print "process message in buffer"
            (message, self.buffer) = (self.buffer[:self.message_length], self.buffer[self.message_length:])
            length = self.message_length
            #self.datagramDecoder.process_datagram(message)
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
    #udp_port = 5000
    #settings
    noDelay = True
    admin = True
#    self.client_id = 0

    def __init__(self):
        self.tcp = None
        self.connected = False
        self.out = SendMessage(self)

        if self.admin:
            self.admin = SendAdminMessage(self)

        #self.encoder = ClientDatagramEncoder(self)
        self.decoder = PacketDecoder(self)

        self.fileno = 0
        self._epoll = select.epoll()

        self.ec = 0

        ClientGlobal.connection = self
        self.connect()

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

            if ClientGlobal.client_id != 0: #reconnection case
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

## Not Needed
class ClientMain:

    def __init__(self):
        self.connection =  TcpConnection()
        self.out = self.connection.out
    def main(self):
        self.connection.out.send_json({'cmd' : 'test' })
        self.out.send_agent_control_state(id=1., d_x=1., d_y=0., d_xa=0., d_za=0., jetpack=0., brake=0.)
        n = 0
        while True:
            self.connection.attempt_recv()
            #can tick
            time.sleep(0.02)
            n += 1
            if n %100 == 0:
                print "tick= %i" % n
