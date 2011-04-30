import socket

import struct
import binascii



class Decoder:
    def __init__(self):
        self.buffer = ''
        self.message_length = 0

    def add_to_buffer(self,data):
        self.buffer += data

    def decode(data):
        if len(self.buffer) < self.message_length:
            return
        elif self.message_length = 0:
            print "decode: get prefix"
            (self.message_length, self.buffer) = self.read_prefix(data)
        if len(self.buffer) <= self.message_length:
            (message, self.buffer) = (self.buffer[:message_length], self.buffer[message_length:])
            self.process_msg(message)

    def read_prefix(self, data):
        prefix = data[0:4]
        (length) = struct.unpack('I', data[0:4])
        return (length, data[4:])

    def process_msg(self, message):
        print "message= " + str(message)

class Encoder:

    def prefix_data(self, data):
        length = len(data)
        prefix = struct.pack('I', length)
        data = prefix + data
        return data

class connection:
    server = '127.0.0.1'
    tcp_port = 5050
    udp_port = 5051

    def __init__(self):
        self.tcp = None
        self.udp = None
        self.encoder = Encoder()
        self.decoder = Decoder()

    def connect()
        self.connect_tcp()
        self.connect_udp()

    def connect_tcp(self):
        TCP_IP = self.server
        TCP_PORT = self.tcp_port
        self.tcp = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.tcp.connect((TCP_IP, TCP_PORT))
        self.tcp.setblocking(0) #test

    def send_tcp(self, MESSAGE):
        self.tcp.send(MESSAGE)

    def get_tcp(self)
        BUFFER_SIZE = 512
        try:
            data = self.tcp.recv(BUFFER_SIZE)
            self.decoder.decode(data)
        except:
            return #in non-blocking, will fail when no data

    def connect_udp()
        pass


#s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#s.connect((TCP_IP, TCP_PORT))
#s.send(MESSAGE)
#data = s.recv(BUFFER_SIZE)
#print "size= " + str(len(data))
#s.close()
