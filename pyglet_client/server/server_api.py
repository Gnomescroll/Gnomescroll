import socket
import struct

import select

import time

import simplejson as json

def pm(id, msg):
    return struct.pack('H',id) +msg

def create_agent_message(agent_id, player_id, x, y, z, x_angle, y_angle):
    t1 = struct.pack('IIfffhh', agent_id, player_id, x, y, z, x_angle, y_angle)
    t2 = pm(2, t1)
    return t2

def agent_position_update(agent_id, tick, x,y,z, vx, vy, vz, ax, ay, az, x_angle, y_angle):
    t1 = struct.pack('II fff fff fff hh', agent_id, tick, x, y, z, vx, vz, vz, ax, ay, az, x_angle, y_angle)
    t2 = pm(3, t1)
    return t2

#           CreateAgentMessage = namedtuple('CreateAgent', 'agent_id', 'player_id', 'x','y','z','x_angle','y_angle')
#            n = CreateAgentMessage(struct.unpack('IIfffhh', datagram))

#200 agent control state
#600 admin json command

class DatagramDecoder:

    def __init__(self, connection):
        self.connection = connection

    def decode(self, message):
        print "decoding datagram"
        (prefix, datagram) = (message[0:2],message[2:])
        (msg_type,) = struct.unpack('H', prefix)

        if msg_type == 0:
            print "test message received"
        if msg_type == 600:
            print "json admin message"
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

class DatagramEncoder:
    def __init__(self, connection):
        self.connection = connection
        pass

    def _pm(id, msg):
        return struct.pack('H',id) +msg

    def create_agent(agent_id, player_id, x, y, z, x_angle, y_angle):
        t1 = struct.pack('IIfffhh', agent_id, player_id, x, y, z, x_angle, y_angle)
        t2 = self._pm(301, t1)
        self.connection.send_tcp(t2)

    def agent_control_state(agent_id, tick, W, S, A, D, JUMP, JETPACK):
        t1 = struct.pack('II BB BB BB BB', agent_id, tick, W, S, A, D, JUMP, JETPACK, x_angle, y_angle)
        t2 = self._pm(200, t1)
        self.connection.send_tcp(t2)

class PacketDecoder:
    def __init__(self,connection):
        self.datagramDecoder = DatagramDecoder(connection)
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


#epoll = select.epoll()
#epoll.register(serversocket.fileno(), select.EPOLLIN)

#events = epoll.poll(1)

import atexit
import socket
import select

class ServerListener:

    IP = '127.0.0.1'
    TCP_PORT = 5055
    UDP_PORT = 5060

    def __init__(self):
        self.tcp = None
        self.tcp_fileno = 0
        self.udp = None
        self.udp_fileno = 0
        self.epoll = select.epoll(2) #2 sockets
        self._setup_tcp_socket()
        atexit.register(self.on_exit)

    def on_exit(self):
        self.tcp.close()
        self.udp.close()

    def _setup_tcp_socket(self):
        print "Setting up TCP socket"
        try:
            self.tcp = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.tcp.bind((self.IP, self.TCP_PORT))
            self.tcp.setsockopt( socket.SOL_SOCKET, socket.SO_REUSEADDR, 1 )
            self.tcp.listen(1)
            self.tcp.setblocking(0)
            self.tcp.listen(1)
            self.tcp_fileno = self.tcp.fileno()
            self.epoll.register(self.tcp.fileno(), select.EPOLLIN)
        except socket.error, (value,message):
            print "TCP socket setup failed: " + str(value) + ", " + message

    def accept_connections(self):
        events = self.epoll.poll(0) #wait upto 0 seconds
        for fileno, event in events:
            if file_no == self.tcp_fileno:
                print "TCP event"
                connection, address = serversocket.accept()
                print 'TCP connection established with:', address
                connection.setblocking(0)
                #hand off connection to connection pool
            if file_no == self.udp_fileno:
                print "UDP event"

    def _listen(self): #this should be a thread, need list of connected clients

        try:
            self.s.listen(1)
            conn, addr = self.s.accept()
            print 'Connected to:', addr
            self.tcp = conn
        except socket.error, (value,message):
            print "listen: socket_error: " + str(value) + ", " + message



## Move into connect handler class ##

    def __init__3(self):
        self.decoder = PacketDecoder(self)
        self.encoder = DatagramEncoder(self)


        #bufsize = self.s.getsockopt( socket.SOL_SOCKET, socket.SO_SNDBUF)
        #print "socket buffer size: %i" % bufsize

    def send(self, MESSAGE):
        try:
            self.send_tcp(MESSAGE)
        except socket.error, (value,message):
            print "send: socket_error: " + str(value) + ", " + message
            self.connected = False
            self._listen()

    def disconnect_tcp(self):
        print "Connection: tcp disconnected by program"
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


#epoll = select.epoll()
#epoll.register(serversocket.fileno(), select.EPOLLIN)
#events = epoll.poll(1)

class ServerListener:

    def __init__(self):
        pass

import select

def ConnectionPool:

    def __init__(self):
        self.epoll = select.epoll()

    def process_events(self):



    connections = {}; requests = {}; responses = {}
    while True:
        events = epoll.poll(1)
            for fileno, event in events:
                if fileno == serversocket.fileno():
                    connection, address = serversocket.accept()
                    connection.setblocking(0)
                    epoll.register(connection.fileno(), select.EPOLLIN)
                    connections[connection.fileno()] = connection
                    requests[connection.fileno()] = b''
                    responses[connection.fileno()] = response
                elif event & select.EPOLLIN:
                    requests[fileno] += connections[fileno].recv(1024)
                if EOL1 in requests[fileno] or EOL2 in requests[fileno]:
                    epoll.modify(fileno, select.EPOLLOUT)
                    print('-'*40 + '\n' + requests[fileno].decode()[:-2])
                elif event & select.EPOLLOUT:
                    byteswritten = connections[fileno].send(responses[fileno])
                    responses[fileno] = responses[fileno][byteswritten:]
                if len(responses[fileno]) == 0:
                    epoll.modify(fileno, 0)
                    connections[fileno].shutdown(socket.SHUT_RDWR)
                elif event & select.EPOLLHUP:
                    epoll.unregister(fileno)
                    connections[fileno].close()
                    del connections[fileno]

#rlist, wlist, elist =select.select( [sock1, sock2], [], [], 5 ), await a read event

M = [
pm(0,"test!"),
pm(1,json.dumps(['test1','test2','test3'])),
create_agent_message(0,1,5,5,5,0,0)
]


test = ServerListener()
while True:
    test.accept_connections()
    time.sleep(1)

#s = ServerInstance()
#s.run()

while False:
    for data in M:
        (prefix) = struct.pack('I', len(data))
        message = prefix + data
        print "sending data: " + str(data)

        s.send(message)  # echo

    time.sleep(15)


#while 1:
#    data = conn.recv(BUFFER_SIZE)
#    if not data: break
#    print "received data:", data
