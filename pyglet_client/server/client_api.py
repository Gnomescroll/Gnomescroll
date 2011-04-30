import socket

import struct
import binascii



class Decoder:
    def __init__(self):
        self.buffer = ''
        self.message_length = 0

    def add_to_buffer(self,data):
        self.buffer += data
        self.attempt_decode()

    def attempt_decode():
        if len(self.buffer) < self.message_length:
            print "decode: need more packets of data to decode message"
            return
        elif len(self.buffer) == 0:
            print "decode: buffer empty"
            return
        elif self.message_length == 0:
            print "decode: get message prefix"
            (self.message_length, self.buffer) = self.read_prefix(data)
        if len(self.buffer) <= self.message_length:
            (message, self.buffer) = (self.buffer[:message_length], self.buffer[message_length:])
            self.message_length = 0
            self.process_msg(message)
            self.attempt_decode()

    def read_prefix(self, data):
        prefix = data[0:4]
        (length) = struct.unpack('I', data[0:4])
        return (length, data[4:])

    def process_msg(self, message):
        print "process message= " + str(message)

class Encoder:

    def prefix_data(self, data):
        length = len(data)
        prefix = struct.pack('I', length)
        data = prefix + data
        return data

class Connection:
    server = '127.0.0.1'
    tcp_port = 5052
    udp_port = 5053

    def __init__(self):
        self.tcp = None
        self.udp = None
        self.encoder = Encoder()
        self.decoder = Decoder()
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

    def send_tcp(self, MESSAGE):
        self.tcp.send(MESSAGE)

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
