import atexit
import select
import socket

# For decoder utils:
import simplejson as json
import struct

class NetServer:
    connectionPool = None
    datagramDecoder = None
    serverListener = None

    #state
    _client_id = 0

    @classmethod
    def generate_client_id(self):
        self._client_id += 1
        return self._client_id

    def __init__(self):
        NetServer.connectionPool = ConnectionPool()
        NetServer.datagramDecoder = DatagramDecoder()
        NetServer.serverListener = ServerListener()

    @classmethod
    def init(self):
        self.connectionPool.init()
        self.datagramDecoder.init()
        self.serverListener.init()

from chat_server import ChatServer
from net_out import SendMessage
from net_event import NetEvent
from game_state import GameStateGlobal

# listens for packets on ports
class ServerListener:

    IP = '127.0.0.1'
    TCP_PORT = 5055
    UDP_PORT = 5060

    def init(self):
        pass
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
                    NetServer.connectionPool.addClient(connection, address) #hand off connection to connection pool
                except socket.error, (value,message):
                    print "ServerListener.accept error: " + str(value) + ", " + message
            if fileno == self.udp_fileno:
                print "UDP event"

# manages TCP stuff and is somehow different from ServerListener and TcpPacketDecoder
class TcpClient:

    def __init__(self, connection, address):
        self.connection = connection
        self.address = address

        self.fileno = connection.fileno()
        self.TcpPacketDecoder = TcpPacketDecoder(self)
        self.sendMessage = SendMessage(self)

        self.player = None
        self.client_id = 0
        self.ec = 0

        #self.sendMessage.send_client_id() #send client an id upon connection

    def identify(self, name):
        if self._set_player_name(name):
            self._set_client_id()
            self.sendMessage.send_client_id(self)
            self._register()

    def _register(self):
        if NetServer.connectionPool.register(self):
            ChatServer.chat.connect(self) # join chat server
            self.player = GameStateGlobal.playerList.join(self.client_id, self.name)  # create player
            print 'Joined chat and created new Player'

    def _set_client_id(self):
        if self.client_id != 0:
            print "ERROR: TcpClient.set_client_id, client_id already assigned"
            return False
        self.client_id = NetServer.generate_client_id()
        return True

    def _set_player_name(self, name):
        try:
            self.name = str(name)
        except ValueError:
            print 'Invalid client name'
            return False
        return True

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
                NetServer.connectionPool.tearDownClient(self)

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
                NetServer.connectionPool.tearDownClient(self)
        else:
            #print "get_tcp: data received, %i bytes" % len(data)
            self.ec = 0
            self.TcpPacketDecoder.add_to_buffer(data)

# manages client connections
class ConnectionPool:

    def init(self):
        pass
    def __init__(self):
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
        if connection.client_id != 0: # remove from chat
            ChatServer.chat.disconnect(connection)
            del self._clients_by_id[connection.client_id]

    def register(self, connection):
        if self._clients_by_id.get(connection.client_id, None) == None:
            self._clients_by_id[connection.client_id] = connection
            print "Connection associated with client_id= %i" % (connection.client_id,)
            return True
        else:
            print "Client id is already registered!"
            self.tearDownClient(connection, duplicate_id = True)
            return False

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


'''
Decoders
'''

# decodes datagram, passes to messageHandler
class DatagramDecoder:

    def init(self):
        pass
    def __init__(self):
        pass

    def decode(self, message, connection):
        prefix, datagram = (message[0:6], message[6:])
        length, msg_type = struct.unpack('I H', prefix)
        if msg_type == 0:
            print "test message received"
        elif msg_type == 1: #client json messages
            try:
                msg = json.loads(datagram)
            except:
                print "JSON DECODING ERROR: %s" % (str(msg),)
                return
            NetEvent.messageHandler.process_json(msg, connection)
        elif msg_type == 2: #client admin messages
            try:
                msg = json.loads(datagram)
            except:
                print "JSON DECODING ERROR: %s" % (str(msg),)
                return
            NetEvent.adminMessageHandler.process_json(msg, connection)
            
# decodes tcp packets
class TcpPacketDecoder:

    def __init__(self, connection):
        self.connection = connection
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
        NetServer.datagramDecoder.decode(message, self.connection)

if __name__ == "__main__":
    print "Run run.py to start server"
