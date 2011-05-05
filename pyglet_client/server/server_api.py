import socket
import struct

import time
import simplejson as json

def pm(id, msg):
    return struct.pack('H',id) +msg

#0 is test message
#1 is json

import math

class PlayerAgent:
    eventOut = None
    GameState = None

    def __init__(self, id, x,y,z,xa, ya):
        assert self.eventOut != None
        assert GameState != None
        [x,y,z] = [float(x),float(y),float(z)]
        self.state = [x,y,z, 0.,0.,0., 0.,0.,0.] #position, velocity, acceleration
        self.xa = xa
        self.ya = ya
        self.id = id

        self.last_control_tick = 0
        self.d_x = 0
        self.d_y = 0
        self.d_xa = 0
        self.d_za = 0
        self.jetpack = 0
        self.brake = 0

    def set_agent_control_state(tick, d_x, d_y, d_xa, d_za, jetpack, brake):
        self.last_control_tick = tick
        self.d_x = d_x #a byte
        self.d_y = d_y #a byte
        self.d_xa = d_xa
        self.d_za = d_za
        self.jetpack = jetpack
        self.brake = brake

    def tick(self):
        [x,y,z, vx,vy,vz, ax,ay,az] = self.state
        tr = 100. #tick rate
        tr2 = tr*tr #tick rate squared
        if z <= 0.:
            az = .10 / tr2
        else:
            az = -0.10 / tr2
        if self.jetpack:
            az += 0.15 / tr2

        xy_speed = 0.1 / tr2
        ax = xy_speed * self.d_x
        ay = xy_speed * self.d_y

        vz += az

        xy_brake = math.pow(.50, 1/(float(tr))) #in percent per second
        vx += ax
        vy += ay
        if self.brake != 0:
            vx *= xy_brake
            vz *= xy_brake

        x += vx
        y += vy
        z += vz

        self.state = [x,y,z, vx,vx,vz, ax,ay,az]
        self.eventOut.agent_state_change(self.id, self.gameState.time, self.state)

class AgentList:
    def __init__(self, gameState):
        self.gameState = gameState
        AgentList.gameState = self.gameState
        self.agents = {}

    def create_agent(self, x,y,z,xa,ya):
        #(x,y,z,xa,ya) = position
        id = self.gameState.new_id()
        agent = PlayerAgent(id, x,y,z,xa,ya)
        self.agents[id] = agent
        print "AgentList: Agent Created, id= %i" %id

    def get_agent(self,id):
        if not self.agents.has_key(id):
            print "Agentlist.set_agent_control_state: Agent does not exist: %i" % id
            return None
        return self.agent[id]

class GameState:

    def __init__(self):
        self.time = 0
        self.id = 0
        self.agentList = AgentList(self)

    def new_id(self):
        self.id += 1
        return self.id

    def tick(self):
        for agent in  self.agentList.agents.values():
            agent.tick()
        self.time += 1
        if self.time % 100 == 0:
            print "time= %i" % (self.time)

import binascii

class EventOut:
    gameState = None

    def __init__(self, pool):
        self.pool = pool
        self.event_packets = []
        self.sendMessage = SendMessage(None)

    def validate_packet(self, packet):
        prefix = self.packet[:4]
        (length,) = struct.unpack('I', prefix)
        print "len1= %i, len2= %%i" % (length, len(prefix))
        #print binascii.b2a_hex(prefix)

    def process_events(self):
        #print "Process Events.num_events = %i" % len(self.event_packets)
        for event_packet in self.event_packets:
            self.validate_packet(event_packet)
            for client in self.pool._client_pool.values():
                client.send(event_packet)
        self.event_packets = []

    def add_json_event(self, dict):
        self.event_packets.append(self.sendMessage.get_json(dict))

    def agent_state_change(self, id, tick, state):
        d = {
            'cmd' : 'agent_position',
            'id' : id,
            'tick' : tick,
            'state': state #is a 9 tuple
           }
        self.add_json_event(d)

class SendMessage:

    def __init__(self, client):
        self.client = client

    def add_prefix(self, id, msg):
        return struct.pack('I H', 4+2+len(msg), id) + msg #length prefix not included in length?
#        return struct.pack('I H', 4+2+len(msg), id) + msg

    def send_json(self, dict):
        self.client.send(self.add_prefix(1, json.dumps(dict)))

    def get_json(self, dict):
        return self.add_prefix(1, json.dumps(dict))

class MessageHandler:

    gameState = None

    def __init__(self, main):
        assert self.gameState != None
        self.main = main

    def process_json(self, msg):
        if not msg.has_key('cmd'):
            print "Json message need cmd parameter: " + str(msg)
            return

        print "MessageHandler.process_json: " + str(msg)
        if msg['cmd'] == 'create_agent':
            self.gameState.create_agent(**msg)
        elif msg['cmd'] == 'agent_control_state':
            id = int(msg['id'])
            agent = GameState.agentList.get_agent(id)
            tick = msg[tick]
            [d_x, d_y, d_xa, d_za, jetpack, brake] = msg['state']
            agent.set_agent_control_state(tick, d_x, d_y, d_xa, d_za, jetpack, brake)
        else:
            print "MessageHandler.process_json: cmd unknown = " + str(msg)

class DatagramDecoder:
    messageHandler = None

    def __init__(self):
        assert self.messageHandler != None

    def decode(self, message):
        print "decoding datagram"
        (prefix, datagram) = (message[0:2],message[2:])
        (msg_type,) = struct.unpack('H', prefix)
        #print "t= " + str(len(prefix))
        #print "t2= " + str(len(datagram))

        if msg_type == 0:
            print "test message received"
        if msg_type == 1:
            print "DatagramDecoder: JSON message"
            try:
                msg = json.loads(datagram)
            except:
                print "JSON DECODING ERROR: " +str(msg)
                return
            self.messageHandler.process_json(msg)

class TcpPacketDecoder:

    def __init__(self):
        self.datagramDecoder = DatagramDecoder()
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
#        elif buff_len < self.message_length:
#            #print "decode: need more packets of data to decode message"
#            return
        elif self.message_length == 0 and buff_len > 4:
            #print "decode: get message prefix"
            (self.message_length, self.buffer) = self.read_prefix()
            #print "prefix length: " + str(self.message_length)
            self.attempt_decode()

        if buff_len >= self.message_length:
            print "process message in buffer"
            (message, self.buffer) = (self.buffer[:self.message_length], self.buffer[self.message_length:])
            self.message_length = 0
            self.process_datagram(message)
            self.attempt_decode()
        else:
            pass
            #print "Need more characters in buffer"

    def read_prefix(self):
        data = self.buffer
        prefix = data[0:4]
        (length,) = struct.unpack('I', data[0:4])
        return (length, data[4:])

    def process_datagram(self, message):
        self.count += 1
        print "processed message count: " +str(self.count)
        self.datagramDecoder.decode(message)

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
                    self.connectionPool.addClient(connection, address) #hand off connection to connection pool
                except socket.error, (value,message):
                    print "ServerListener.accept error: " + str(value) + ", " + message
            if fileno == self.udp_fileno:
                print "UDP event"

class TcpClient:
    pool = None

    def __init__(self, connection, address):
        assert self.pool != None
        self.connection = connection
        self.address = address

        self.fileno = connection.fileno()
        self.TcpPacketDecoder = TcpPacketDecoder()
        self.sendMessage = SendMessage(self)

        self.player_id = 0
        self.client_id = 0
        self.ec = 0

    def send(self, MESSAGE):
        try:
            if False:
                length = len(MESSAGE)
                sent = self.connection.send(MESSAGE)
                if length != sent:
                    print "ALL DATA NOT SENT!"
            else:
                self.connection.sendall(MESSAGE)
        except socket.error, (value,message):
            print "TcpClient.send error: " + str(value) + ", " + message
            if value == 32:  #connection reset by peer
                self.pool.tearDownClient(self.fileno)

    def close(self):
        print "TcpClient.close : connection closed gracefully"
        self.connection.close()

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
                self.pool.tearDownClient(self.fileno)
        else:
            print "get_tcp: data received, %i bytes" % len(data)
            self.ec = 0
            self.TcpPacketDecoder.add_to_buffer(data)

class ConnectionPool:

    def __init__(self, main, messageHandler):
        #parents
        self.main = main
        self.messageHandler = messageHandler
        TcpClient.pool = self

        #children
        self.datagramDecoder = DatagramDecoder()
        #local
        self._epoll = select.epoll()
        self._client_count = 0
        self._client_pool = {}

        atexit.register(self.on_exit)

    def on_exit(self):
        self._epoll.close()
        for client in self._client_pool.values():
            client.close()

    def addClient(self, connection, address, type = 'tcp'):
        self._client_count += 1
        if type == 'tcp':
            client =  TcpClient(connection, address)
            self._epoll.register(client.fileno, select.EPOLLIN | select.EPOLLHUP ) #register client
            self._client_pool[client.fileno] = client #save client

    def tearDownClient(self, fileno):
        self._epoll.unregister(fileno)
        self._client_pool[fileno].close()
        del self._client_pool[fileno] #remove from client pool

    def process_events(self):
        events = self._epoll.poll(0)
        for fileno, event in events:
            print "(event, fileno) = %i, %i" % (event, fileno)
            if event & select.EPOLLIN:
                assert self._client_pool.has_key(fileno)
                self._client_pool[fileno].receive()
            elif event & select.EPOLLOUT:
                pass
                print "EPOLLOUT event?"
            elif event & select.EPOLLHUP:
                print "EPOLLHUP: teardown socket"
                self.tearDownClient(fileno)
            else:
                print "EPOLLOUT weird event: %i" % event
#rlist, wlist, elist =select.select( [sock1, sock2], [], [], 5 ), await a read event

class Main:

    def __init__(self):
        self.gameState = GameState()
        MessageHandler.gameState = self.gameState
        self.messageHandler = MessageHandler(self)
        DatagramDecoder.messageHandler = self.messageHandler #set global
        self.connectionPool = ConnectionPool(self, self.messageHandler)
        self.serverListener = ServerListener(self.connectionPool)
        self.eventOut = EventOut(self.connectionPool)
        self.eventOut.gameState = self.gameState
        PlayerAgent.eventOut = self.eventOut
        PlayerAgent.gameState = self.gameState
        #globals

    def run(self):
        print "Server Started"
        print "Create Agent"
        self.gameState.agentList.create_agent(0,0,0,0,0)
        while True:
            self.serverListener.accept() #accept incoming connections
            self.connectionPool.process_events() #check for new data
            self.gameState.tick()
            self.eventOut.process_events()
            time.sleep(.01)

if __name__ == "__main__":

    main = Main()
    main.run()
