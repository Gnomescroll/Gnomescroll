import socket

import struct
import binascii

import simplejson as json

from collections import namedtuple

msg_types = [
(1, 'json'),
(2, 'create_agent'),
(3, 'agent_position_update'),
]

class DatagramDecoder:

    def __init__(self):
        pass

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
        if msg_type == 4:
            #latency estimation



class PacketDecoder:
    def __init__(self):
        self.datagramDecoder = DatagramDecoder()
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

class PacketEncoder:

    def prefix_data(self, data):
        length = len(data)
        prefix = struct.pack('I', length)
        data = prefix + data
        return data

class Connection:
    server = '127.0.0.1'
    tcp_port = 5053
    udp_port = 5054

    def __init__(self):
        self.tcp = None
        self.udp = None
        self.encoder = PacketEncoder()
        self.decoder = PacketDecoder()
        self.connect()

    def connect(self):
        self.connect_tcp()
        self.connect_udp()

    def connect_tcp(self):
        TCP_IP = self.server
        TCP_PORT = self.tcp_port
        self.tcp = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.tcp.connect((TCP_IP, TCP_PORT))
        self.tcp.setblocking(0) #test
        print "Connection: tcp connected"

    def disconnect_tcp(self):
        print "Connection: tcp disonnected by program"
        self.tcp.close()

    def send_tcp(self, MESSAGE):
        #self.tcp.send(MESSAGE)
        self.tcp.sendall(MESSAGE)

    def get_tcp(self):
        BUFFER_SIZE = 512
        try:
            data = self.tcp.recv(BUFFER_SIZE)
            print "get_tcp: data received"
            self.decoder.add_to_buffer(data)
        except socket.error, (value,message):
            print "get_tcp: socket error " + str(value) + ", " + message
            return #in non-blocking, will fail when no data

    def connect_udp(self):
        pass


#s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#s.connect((TCP_IP, TCP_PORT))
#s.send(MESSAGE)
#data = s.recv(BUFFER_SIZE)
#print "size= " + str(len(data))
#s.close()
