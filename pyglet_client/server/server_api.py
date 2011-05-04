import socket
import struct

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

#   CreateAgentMessage = namedtuple('CreateAgent', 'agent_id', 'player_id', 'x','y','z','x_angle','y_angle')
#   n = CreateAgentMessage(struct.unpack('IIfffhh', datagram))

#0 is test message
#1 is json

class MessageHandler:
    def __init__(self, main):
        self.main = main

    def process_json(dict):
        print "MessageHandler.process_json: " + str(dict)

class DatagramDecoder:

    def __init__(self, messageHandler):
        self.messageHandler = messageHandler
    def decode(self, message):
        print "decoding datagram"
        (prefix, datagram) = (message[0:2],message[2:])
        (msg_type,) = struct.unpack('H', prefix)

        if msg_type == 0:
            print "test message received"
        if msg_type == 1:
            print "Generatic JSON message"
            dict = json.loads(message[2:])
            self.messageHandler.process_json(dict)
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
    def __init__(self, client):
        self.connection = client
        pass

    def json(self, dict):
        #t = json.dumps(dict)
        #t = self._pm(1, t)
        #t = add_length_prefix(self, t)
        self.client.send(self.add_prefix(1, json.dumps(dict)))

    def add_prefix(self, id, msg):
        return struct.pack('H I', (id, 2+len(msg))) + msg

#    def add_length_prefix(self, msg):
        return struct.pack('I', len(msg)) + msg

#    def _pm(self, id, msg):
#       return struct.pack('H',id) +msg

    def _create_agent(self, agent_id, player_id, x, y, z, x_angle, y_angle):
        t1 = struct.pack('IIfffhh', agent_id, player_id, x, y, z, x_angle, y_angle)
        t2 = self._pm(301, t1)
        self.connection.send_tcp(t2)

    def _agent_control_state(self, agent_id, tick, W, S, A, D, JUMP, JETPACK):
        t1 = struct.pack('II BB BB BB BB', agent_id, tick, W, S, A, D, JUMP, JETPACK, x_angle, y_angle)
        t2 = self._pm(200, t1)
        self.connection.send_tcp(t2)

class TcpPacketDecoder:
    def __init__(self, client, messageHandler):
        #self.client = client
        self.datagramDecoder = DatagramDecoder(client)
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
            self.process_datagram(message)
            self.attempt_decode()
        else:
            print "Need more characters in buffer"

    def read_prefix(self):
        data = self.buffer
        prefix = data[0:4]
        (length,) = struct.unpack('I', data[0:4])
        return (length, data[4:])

    def process_datagram(self, message):
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

    def __init__(self, connectionPool):
        self.connectionPool = connectionPool
        self.tcp = None
        self.tcp_fileno = 0
        self.udp = None
        self.udp_fileno = 0
        self.epoll = select.epoll(2) #2 sockets
        self._setup_tcp_socket()
        atexit.register(self.on_exit)

    def on_exit(self):
        self.tcp.close()
        #self.udp.close()

    def _setup_tcp_socket(self):
        print "Setting up TCP socket"
        try:
            self.tcp = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.tcp.setsockopt( socket.SOL_SOCKET, socket.SO_REUSEADDR, 1 )
            self.tcp.bind((self.IP, self.TCP_PORT))
            self.tcp.listen(1)
            self.tcp.setblocking(0)
            self.tcp.listen(1)
            self.tcp_fileno = self.tcp.fileno()
            self.epoll.register(self.tcp.fileno(), select.EPOLLIN)
            print "TCP socket listening on port %i" % self.TCP_PORT
        except socket.error, (value,message):
            print "TCP socket setup failed: " + str(value) + ", " + message

    def accept(self):
        events = self.epoll.poll(0) #wait upto 0 seconds
        for fileno, event in events:
            if fileno == self.tcp_fileno:
                try:
                    connection, address = self.tcp.accept()
                    print 'TCP connection established with:', address
                    connection.setblocking(0)
                    #cc = self.ClientConnection(connection, address) ##create connection
                    self.connectionPool.addConnection(connection, address) #hand off connection to connection pool
                except socket.error, (value,message):
                    print "ServerListener.accept error: " + str(value) + ", " + message
            if fileno == self.udp_fileno:
                print "UDP event"



### PURGE

#epoll = select.epoll()
#epoll.register(serversocket.fileno(), select.EPOLLIN)
#events = epoll.poll(1)

class TcpClient:

    def __init__(self, pool, messageHandler, connection, address):
        self.pool = pool
        self.messageHandler = messageHandler

        self.connection = connection
        self.address = address

        self.fileno = connection.fileno()
        self.TcpPacketDecoder = TcpPacketDecoder(self)

        self.player_id = 0
        self.client_id = 0

    def send(self, MESSAGE):
        try:
            self.connection.sendall(MESSAGE)
        except socket.error, (value,message):
            print "TcpClient.send error: " + str(value) + ", " + message

    def close(self):
        print "TcpClient.close : connection cloesed by program"
        self.tcp.close()

    def receive(self):
        BUFFER_SIZE = 4096
        try:
            data = self.connection.recv(BUFFER_SIZE)
            print "get_tcp: data received"
            self.TcpPacketDecoder.add_to_buffer(data)
        except socket.error, (value,message):
            print "TcpClient.get: socket error %i, %s" % (value, message)

class ConnectionPool:

    def __init__(self, main, messageHandler):
        #parents
        self.main = main
        self.messageHandler = messageHandler
        #children
        self.datagramDecoder = DatagramDecoder(self.messageHandler)
        #local
        self._epoll = select.epoll()
        self._client_count = 0
        self._client_pool = {}

        atexit.register(self.on_exit)

    def on_exit(self):
        self._epoll.close()
        for client in self._client_pool:
            client.close()

    def addClient(self, connection, messageHandler, address, type = 'tcp'):
        self._client_count += 1
        if type == 'tcp':
            client =  TcpClient(self, connection, address)
            self._epoll.register(client.fileno, select.EPOLLIN) #register client
            self._client_pool[client.fileno] = client #save client

    def process_events(self):
        #connections = {}; requests = {}; responses = {}
        while True:
            events = self._epoll.poll(1)
            for fileno, event in events:
                if event & select.EPOLLIN:
                    assert fileno in self._client_pool.has_key(fileno)
                    self._client_pool[fileno].receive()
                elif event & select.EPOLLOUT:
                    print "EPOLLOUT event?"
                    #byteswritten = connections[fileno].send(responses[fileno])
                    #responses[fileno] = responses[fileno][byteswritten:]
                elif event & select.EPOLLHUP:
                    print "EPOLLHUP: teardown socket"
                    epoll.unregister(fileno)
                    self._client_pool[fileno].close()
                    del self._client_pool[fileno] #remove from client pool

#rlist, wlist, elist =select.select( [sock1, sock2], [], [], 5 ), await a read event

class Main:

    def __init__(self):
        self.messageHandler = MessageHandler(self)
        self.connectionPool = ConnectionPool(self, self.messageHandler)
        self.ServerListener = ServerListener(self.connectionPool)

    def run(self):
        while True:
            self.ServerListener.accept() #accept incoming connections
            self.connectionPool.process_events() #check for new data
            time.sleep(.025)

if __name__ == "__main__":

    main = Main()
    main.run()

    M = [
    pm(0,"test!"),
    pm(1,json.dumps(['test1','test2','test3'])),
    create_agent_message(0,1,5,5,5,0,0)
    ]

    connectionPool = connectionPool()
    test = ServerListener(connectionPool)
    while True:
        test.accept()
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
