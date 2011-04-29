import socket

from struct import *

class Decoder:

    def __init__(self):
        self.current_message = None
        self.message_length_remaining = None

    def decode(data):
        if self.message_length_remaining = None:
            (data,length) = self.read_prefix(data)

    def read_prefix(self, data):


class Encoder:


class connection:
    server = '127.0.0.1'
    tcp_port = 5050
    udp_port = 5051

    def __init__(self):
        self.tcp = None
        self.udp = None
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

print "received data:", data

    def connect_udp()
        pass


s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((TCP_IP, TCP_PORT))
s.send(MESSAGE)
data = s.recv(BUFFER_SIZE)
print "size= " + str(len(data))
s.close()
