import socket
import struct
import binascii

from collections import namedtuple

import simplejson as json

class SendMessage:

    def __init__(self, client):
        self.client = client

    def json(self, dict):
        self.client.send(self.add_prefix(1, json.dumps(dict)))

    def add_prefix(self, id, msg):
        return struct.pack('I H', 4+2+len(msg), id) + msg

    ### agent messages

    def send_agent_control_state(self, id, d_x, d_y, d_xa, d_za, jetpack, brake):
        d = {
            'cmd' : 'agent_control_state',
            'id' : id,
            'tick' : 0,
            'state': [d_x, d_y, d_xa, d_za, jetpack, brake]
           }
        self.json(d)

class ClientDatagramEncoder:

    def __init__(self, connection):
        self.connection = connection

    def _pm(id, msg):
        return struct.pack('H',id) +msg

    def admin_create_agent(agent_id, player_id, x, y, z, x_angle, y_angle):
        t1 = struct.pack('IIfffhh', agent_id, player_id, x, y, z, x_angle, y_angle)
        t2 = self._pm(301, t1)
        self.connection.send_tcp(t2)

    def admin_create_agent2(player_id,x,y,z,x_angle=0, y_angle = 0):
        d = {
                'cmd':'create_agent',
                'type':'admin',
                'value':(x,y,z, x_angle, y_angle),
            }
        self.connection.send_tcp(self._pm(600, json.dump(d)))


    def agent_control_state(agent_id, tick, W, S, A, D, JUMP, JETPACK):
        t1 = struct.pack('II BB BB BB BB', agent_id, tick, W, S, A, D, JUMP, JETPACK, x_angle, y_angle)
        t2 = self._pm(200, t1)
        self.connection.send_tcp(t2)

import binascii

class ClientDatagramDecoder:

    def __init__(self, connection):
        self.connection = connection

    def process_datagram(self, message, length):
        print "decoding datagram"
        (prefix, datagram) = (message[0:2],message[2:])
        (msg_type,) = struct.unpack('H', prefix)

        if msg_type == 0:
            print "test message received"
        if msg_type == 1:
            try:
                print "json message"
                msg = json.loads(datagram)
            except:
                print "error decoding: len = %i, message_length= %i" % (len(datagram), length)
                print str(datagram)
                print binascii.b2a_hex(datagram)
            print str(msg)

class PacketDecoder:
    def __init__(self,connection):
        self.datagramDecoder = ClientDatagramDecoder(connection)
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
        elif buff_len < self.message_length:
            #print "decode: need more packets of data to decode message"
            return
        elif self.message_length == 0 and buff_len > 4:
            #print "decode: get message prefix"
            (self.message_length, self.buffer) = self.read_prefix()
            #print "prefix length: " + str(self.message_length)
            self.attempt_decode()

        if buff_len >= self.message_length:
            print "process message in buffer"
            (message, self.buffer) = (self.buffer[:self.message_length], self.buffer[self.message_length:])
            length = self.message_length
            self.message_length = 0
            #self.process_datagram(message)
            self.datagramDecoder.process_datagram(message, length)
            self.attempt_decode()
        else:
            pass
            #print "Need more characters in buffer"

    def read_prefix(self):
        data = self.buffer
        prefix = data[0:4]
        (length,) = struct.unpack('I', data[0:4])
        return (length, data[4:])

    def process_msg(self, message):
        self.count += 1
        print "processed message count: " +str(self.count)
        self.datagramDecoder.decode(message)

import select
class TcpConnection:
    server = '127.0.0.1'
    tcp_port = 5055
    udp_port = 5000
    #settings
    noDelay = True


    def __init__(self):
        self.tcp = None
        self.connected = False
        self.out = SendMessage(self)
        #self.encoder = ClientDatagramEncoder(self)
        self.decoder = PacketDecoder(self)

        self.fileno = 0
        self._epoll = select.epoll()

        self.ec = 0
        self.connect()

    def connect(self):
        TCP_IP = self.server
        TCP_PORT = self.tcp_port
        try:
            self.tcp = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.tcp.connect((TCP_IP, TCP_PORT))

            if self.noDelay == True:
                self.tcp.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
            self.tcp.setblocking(0) #should be blocking?
            self._epoll.register(self.tcp.fileno(), select.EPOLLIN | select.EPOLLHUP | select.EPOLLOUT )
            print "Connection: tcp connected"
            self.fileno = self.tcp.fileno()
            self.connected = True
        except socket.error, (value,message):
            print "Connection failed: socket error " + str(value) + ", " + message
            self.connected = False

    def disconnect(self):
        print "Connection: tcp disonnected by program"
        self.connected = False
        self._epoll.unregister(self.fileno)
        self.tcp.close()

    def send(self, MESSAGE):
        print "send len: " + str(len(MESSAGE))
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
                print "Read Event"
                self.recv()
            elif event & select.EPOLLOUT:
                pass #ready to write
            else:
                print "Strange Epoll Event: %i" % eventOut

    def receive(self):
        BUFFER_SIZE = 4096
        try:
            data = self.connection.recv(BUFFER_SIZE)
        except socket.error, (value,message):
            print "TcpClient.get: socket error %i, %s" % (value, message)
            data = ''
        if len(data) == 0: #if we read three times and get no data, close socket
            #print "tcp data: empty read"
            self.ec += 1
            if self.ec > 3:
                self.disconnect()
        else:
            print "get_tcp: data received, %i bytes" % len(data)
            self.ec = 0
            self.TcpPacketDecoder.add_to_buffer(data)

    def recv_DEPRICATED(self):
        BUFFER_SIZE = 2048
        try:
            data = self.tcp.recv(BUFFER_SIZE)
            print "get_tcp: data received"
            self.decoder.add_to_buffer(data)
        except socket.error, (value,message):
            print "get_tcp: socket error " + str(value) + ", " + message
            return #in non-blocking, will fail when no data

class ClientMain:

    def __init__(self):
        self.connection =  TcpConnection()

    def main(self):
        self.connection.out.json({'cmd' : 'test' })
        n = 0
        while True:
            self.connection.attempt_recv()
            #can tick
            time.sleep(0.01)
            n += 1
            if n %100 == 0:
                print "tick= %i" % n


import time
if __name__ == "__main__":

    x = ClientMain()
    x.main()

    if False:
        print "Running client as program"
        x= TcpConnection()
        #x.connect()
        x.out.json({'cmd' : 'test' })
        time.sleep(3)
        x.disconnect()
