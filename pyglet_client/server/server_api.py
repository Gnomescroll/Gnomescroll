import socket
import struct

import simplejson as json

import binascii

import atexit
import socket
import select

class ServerGlobal:
    connectionPool = None
    eventOut = None
    messageOut = None
    messageHandler = None
    adminMessageHandler = None
    datagramDecoder = None
    serverListener = None
    #state
    _client_id = 0

    @classmethod
    def generate_client_id(self):
        self._client_id += 1
        return self._client_id

    def __init__(self):
        ServerGlobal.connectionPool = ConnectionPool()
        ServerGlobal.eventOut = EventOut()
        ServerGlobal.messageOut = MessageOut()
        ServerGlobal.messageHandler = MessageHandler()
        ServerGlobal.adminMessageHandler = AdminMessageHandler()
        ServerGlobal.datagramDecoder = DatagramDecoder()
        ServerGlobal.serverListener = ServerListener()

    @classmethod
    def init(self):
        assert self.datagramDecoder != None
        self.connectionPool.init()
        self.eventOut.init()
        self.messageOut.init()
        self.messageHandler.init()
        self.adminMessageHandler.init()
        self.datagramDecoder.init()
        self.serverListener.init()

from game_state import GameStateGlobal

# sends event packets to all clients
class EventOut:
    gameState = None
    pool = None

    def init(self):
        self.pool = ServerGlobal.connectionPool
        self.gameState = GameStateGlobal.gameState
        assert self.pool != None
        assert self.gameState != None

    def __init__(self):
        ServerGlobal.eventOut = self
        self.event_packets = []

    def process_events(self):
        #print "Process Events.num_events = %i" % len(self.event_packets)
        for event_packet in self.event_packets:
            for client in self.pool._client_pool.values():
                client.send(event_packet)
        self.event_packets = []

    def add_json_event(self, dict):
        self.event_packets.append(SendMessage.get_json(dict))

    def agent_state_change(self, id, tick, state):
        d = {
            'cmd'  : 'agent_position',
            'id'   : id,
            'tick' : tick,
            'state': state #is a 9 tuple
        }
        self.add_json_event(d)

#this is global message out across the connection pool
class MessageOut:
    def init(self):
        pass
    def __init__(self):
        ServerGlobal.messageOut = self

class SendMessage: #each connection has one of these
    @classmethod
    def add_prefix(self, id, msg):
        return struct.pack('I H', 4+2+len(msg), id) + msg #length prefix not included in length?
    @classmethod
    def get_json(self, dict):
        return self.add_prefix(1, json.dumps(dict))
    def __init__(self, client):
        self.client = client
    def send_json(self, dict):
        self.client.send(self.add_prefix(1, json.dumps(dict)))

    ## messages go out immediately
    def send_client_id(self):
        print "Send client id"
        d = {
            'cmd'  : 'send_client_id',
            'id'   : ServerGlobal.generate_client_id(),
        }
        self.send_json(d)

    def send_chunk_list(self):
        d = {
            'cmd'  : 'chunk_list',
            'list': GameStateGlobal.terrainMap.get_chunk_list(),
        }
        self.send_json(d)

    def send_chunk(self, x, y z):
        chunk_str = GameStateGlobal.terrainMap.get_packed_chunk(x,y,z)
        if chunk_str != '':
            self.client.send(self.add_prefix(3, chunk_str))
        else:
            print "send chunk error: chunk id invalid"

# routes messages by msg.cmd
class MessageHandler:
    gameState = None
    chat = None #put this somewhere

    def init(self):
        self.gameState = GameStateGlobal.gameState
        assert self.gameState != None
        #assert self.chat != None

    def __init__(self):
        ServerGlobal.messageHandler = self

    def process_json(self, msg, connection):
        cmd = msg.get('cmd', None)
        #print "MessageHandler.process_json: " + str(msg)
        if cmd == 'create_agent':
            self.gameState.create_agent(**msg)
        elif cmd == 'agent_control_state':
            self.agent_control_state(msg)
        elif cmd == 'chat':
            self.chat.received(msg)
        elif cmd == 'send_client_id': #Setup client connection
            connection.set_client_id(int(msg['id']))
            #print "Client Assigned id= %i" % (connection.client_id,)
        elif cmd == 'request_chunk_list':
            self.send_chunk_list(msg, connection)
        elif cmd == 'request_chunk':
            self.request_chunk(msg, connection)
        else:
            print "MessageHandler.process_json: cmd unknown = %s" % (str(msg),)

    def agent_control_state(self, msg):
        try:
            id = int(msg.get('id', None))
        except TypeError:
            print 'msg.cmd == agent_control_state, but msg.id missing. MSG: %s' % (str(msg),)
            return
        except ValueError:
            print 'msg.cmd == agent_control_state, but msg.id is not an int. MSG: %s' % (str(msg),)
            return
        agent = self.gameState.agentList.get_agent(id)
        tick = msg.get('tick', None)
        d_x, d_y, d_xa, d_za, jetpack, brake = msg.get('state', [None for i in range(6)])
        agent.set_agent_control_state(tick, d_x, d_y, d_xa, d_za, jetpack, brake)

    def send_chunk_list(self, msg, connection):
        connection.sendMessage.send_chunk_list()

    def request_chunk(self, msg, connection):
        (x,y,z) = msg.get('value')
        connection.sendMessage.send_chunk(x,y,z)


class AdminMessageHandler:
    gameState = None

    def init(self):
        self.gameState = GameStateGlobal.gameState
        assert self.gameState != None

    def __init__(self):
        ServerGlobal.AdminMessageHandler = self

    def process_json(self, msg, connection):
        cmd = msg.get('cmd', None)
        if cmd == "set_map":
            l = msg.get('list', [])
            terrainMap = gameState.terrainMap
            for (x,y,z,value) in  l:
                terrainMap.set(x,y,z,value)
        else:
            print "Admin Message Handler, uncaught message"

# decodes datagram, passes to messageHandler
class DatagramDecoder:
    messageHandler = None

    def init(self):
        self.messageHandler = ServerGlobal.messageHandler
        assert self.messageHandler != None

    def __init__(self):
        print "dataqgram decoder init"
        pass

    def decode(self, message, connection):
        prefix, datagram = (message[0:6], message[6:])
        (length, msg_type) = struct.unpack('I H', prefix)
        if msg_type == 0:
            print "test message received"
        elif msg_type == 1: #client json messages
            try:
                msg = json.loads(datagram)
            except:
                print "JSON DECODING ERROR: %s" % (str(msg),)
                return
            self.messageHandler.process_json(msg, connection)
        elif msg_type == 2: #client admin messages
            try:
                msg = json.loads(datagram)
            except:
                print "JSON DECODING ERROR: %s" % (str(msg),)
                return
            self.adminMessageHandler.process_json(msg, connection)
# decodes tcp packets
class TcpPacketDecoder:

    def __init__(self, connection):
        self.connection = connection
        self.datagramDecoder = ServerGlobal.datagramDecoder
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
            message, self.buffer = (self.buffer[:self.message_length], self.buffer[self.message_length:])
            length = self.message_length
            self.process_datagram(message)
            self.message_length = 0
            self.attempt_decode()

    def read_prefix(self):
        data = self.buffer
        prefix = data[0:4]
        (length,) = struct.unpack('I', data[0:4])
        return length

    def process_datagram(self, message):
        self.count += 1
        #print "processed message count: " +str(self.count)
        self.datagramDecoder.decode(message, self.connection)

# listens for packets on ports
class ServerListener:

    IP = '127.0.0.1'
    TCP_PORT = 5055
    UDP_PORT = 5060

    connectionPool = None

    def init(self):
        self.connectionPool = ServerGlobal.connectionPool
        assert self.connectionPool != None
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
        #self.udp.close()

    def _setup_tcp_socket(self):
        print "Setting up TCP socket"
        try:
            self.tcp = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.tcp.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            self.tcp.bind((self.IP, self.TCP_PORT))
            self.tcp.listen(1)
            self.tcp.setblocking(0)
            self.tcp.listen(1)
            self.tcp_fileno = self.tcp.fileno()
            self.epoll.register(self.tcp.fileno(), select.EPOLLIN)
            print "TCP socket listening on port %i" % (self.TCP_PORT,)
        except socket.error, (value, message):
            print "TCP socket setup failed: %s, %s" % (str(value), message,)

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

# manages TCP stuff and is somehow different from ServerListener and TcpPacketDecoder
class TcpClient:
    pool = None

    def __init__(self, connection, address):
        assert self.pool != None
        self.connection = connection
        self.address = address

        self.fileno = connection.fileno()
        self.TcpPacketDecoder = TcpPacketDecoder(self)
        self.sendMessage = SendMessage(self)

        self.player_id = 0
        self.client_id = 0
        self.ec = 0

        self.sendMessage.send_client_id() #send client an id upon connection

    def set_client_id(self, id):
        if self.client_id != 0:
            print "ERROR: TcpClient.set_client_id, client_id already assigned"
            return
        self.client_id = id
        self.pool.register_client_id(self)

    def send(self, MESSAGE):
        try:
            if False:
                length = len(MESSAGE)
                sent = self.connection.send(MESSAGE)
                if length != sent:
                    print "ALL DATA NOT SENT!"
            else:
                self.connection.sendall(MESSAGE)
        except socket.error, (value, message):
            print "TcpClient.send error: " + str(value) + ", " + message
            if value == 32:  #connection reset by peer
                self.pool.tearDownClient(self)

    def close(self):
        print "TcpClient.close : connection closed gracefully"
        self.connection.close()

    def receive(self):
        BUFFER_SIZE = 4096
        try:
            data = self.connection.recv(BUFFER_SIZE)
        except socket.error, (value, message):
            print "TcpClient.get: socket error %i, %s" % (value, message)
            data = ''
        if len(data) == 0: #if we read three times and get no data, close socket
            #print "tcp data: empty read"
            self.ec += 1
            if self.ec > 3:
                self.pool.tearDownClient(self.fileno)
        else:
            #print "get_tcp: data received, %i bytes" % len(data)
            self.ec = 0
            self.TcpPacketDecoder.add_to_buffer(data)

# manages client connections
class ConnectionPool:
    messageHandler = None
    datagramDecoder = None

    def init(self):
        self.messageHandler = ServerGlobal.messageHandler
        assert self.messageHandler != None
        self.datagramDecoder = ServerGlobal.datagramDecoder
        assert self.datagramDecoder != None

    def __init__(self):
        ServerGlobal.connectionPool = self
        #parents
        TcpClient.pool = self
        #local
        self._epoll = select.epoll()
        self._client_count = 0
        self._client_pool = {} #clients by fileno
        self._clients_by_id = {}
        atexit.register(self.on_exit)

    def on_exit(self):
        self._epoll.close()
        for client in self._client_pool.values():
            client.close()

    def addClient(self, connection, address, type='tcp'):
        self._client_count += 1
        if type == 'tcp':
            client =  TcpClient(connection, address)
            self._epoll.register(client.fileno, select.EPOLLIN or select.EPOLLHUP) #register client
            self._client_pool[client.fileno] = client #save client

    def tearDownClient(self, connection, duplicate_id = False):
        fileno = connection.fileno
        self._epoll.unregister(fileno)
        self._client_pool[fileno].close()
        del self._client_pool[fileno] #remove from client pool
        if connection.client_id != 0:
            del self._clients_by_id[connection.client_id]

    def register_client_id(self, connection):
        if self._clients_by_id.get(connection.client_id, None) == None:
            self._clients_by_id[connection.client_id] = connection
            print "Connection associated with client_id= %i" % (connection.client_id,)
        else:
            print "Client id is already registered!"
            self.tearDownClient(connection, duplicate_id = True)

    def process_events(self):
        events = self._epoll.poll(0)
        for fileno, event in events:
            #print "(event, fileno) = %i, %i" % (event, fileno)
            if event and select.EPOLLIN:
                assert self._client_pool.has_key(fileno)
                self._client_pool[fileno].receive()
            elif event and select.EPOLLOUT:
                pass
                print "EPOLLOUT event?"
            elif event and select.EPOLLHUP:
                print "EPOLLHUP: teardown socket"
                self.tearDownClient(self._client_pool[fileno])
            else:
                print "EPOLLOUT weird event: %i" % (event,)
#rlist, wlist, elist =select.select( [sock1, sock2], [], [], 5 ), await a read event

if __name__ == "__main__":
    print "Run run.py to start server"
