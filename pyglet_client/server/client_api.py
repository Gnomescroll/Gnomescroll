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

class ClientDatagramDecoder:

    def __init__(self, connection):
        self.connection = connection

    def decode(self, message):
        print "decoding datagram"
        (prefix, datagram) = (message[0:2],message[2:])
        (msg_type,) = struct.unpack('H', prefix)

        if msg_type == 0:
            print "test message received"
        if msg_type == 1:
            print "json message"
            msg = json.loads(message[2:])
            print str(msg)
        if msg_type == 2: #
            #CreateAgent
            (agent_id, player_id, x, y, z, x_angle, y_angle) = struct.unpack('IIfffhh', datagram)
            print str((agent_id, player_id, x, y, z, x_angle, y_angle))
            #n = CreateAgentMessage(struct.unpack('IIfffhh', datagram))
            #print str(n)
        if msg_type == 3:
            #agentPositionUpdate
            (agent_id, tick, x, y, z, vx, vz, vz, ax, ay, az, x_angle, y_angle) = struct.unpack('II fff fff fff hh', datagram)
            print str(agent_id, tick, x, y, z, vx, vz, vz, ax, ay, az, x_angle, y_angle)

        if msg_type == 99:
            #latency estimation
            (time, tick) = struct.unpack('I f', datagram)
            print str((time, tick))
            #immediately send message 5 back
        if msg_type == 100:
            (time, tick) = struct.unpack('I f', datagram)
            #return packet for latency testing
            print str((time, tick))

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
        if len(self.buffer) < self.message_length:
            print "decode: need more packets of data to decode message"
            return
        if len(self.buffer) == 0:
            print "decode: buffer empty"
            return
        if self.message_length == 0:
            print "decode: get message prefix"
            (self.message_length, self.buffer) = self.read_prefix()
            print "prefix length: " + str(self.message_length)

        if len(self.buffer) >= self.message_length:
            print "process message in buffer"
            (message, self.buffer) = (self.buffer[:self.message_length], self.buffer[self.message_length:])
            self.message_length = 0
            self.process_msg(message)
            self.attempt_decode()
        else:
            print "Need more characters in buffer"

    def read_prefix(self):
        data = self.buffer
        prefix = data[0:4]
        (length,) = struct.unpack('I', data[0:4])
        return (length, data[4:])

    def process_msg(self, message):
        self.count += 1
        print "processed message count: " +str(self.count)
        self.datagramDecoder.decode(message)

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
            print "Connection: tcp connected"
            self.connected = True
        except socket.error, (value,message):
            print "Connection failed: socket error " + str(value) + ", " + message
            self.connected = False

    def disconnect(self):
        print "Connection: tcp disonnected by program"
        self.tcp.close()

    def send(self, MESSAGE):
        print "send len: " + str(len(MESSAGE))
        if self.connected == True:
            try:
                self.tcp.sendall(MESSAGE)
            except socket.error, (value,message):
                print "Connection failed: socket error " + str(value) + ", " + message
        else:
            print "TcpConnection.send: Socket is not connected!"

    def recv(self):
        BUFFER_SIZE = 512
        try:
            data = self.tcp.recv(BUFFER_SIZE)
            print "get_tcp: data received"
            self.decoder.add_to_buffer(data)
        except socket.error, (value,message):
            print "get_tcp: socket error " + str(value) + ", " + message
            return #in non-blocking, will fail when no data

import time
if __name__ == "__main__":
    print "Running client as program"
    x= TcpConnection()
    #x.connect()
    x.out.json({'message' : 'test' })
    time.sleep(3)
    x.disconnect()
#s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#s.connect((TCP_IP, TCP_PORT))
#s.send(MESSAGE)
#data = s.recv(BUFFER_SIZE)
#print "size= " + str(len(data))
#s.close()
