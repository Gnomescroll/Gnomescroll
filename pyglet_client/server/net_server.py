import atexit
import select
import socket

# For decoder utils:
import simplejson as json
import struct

from opts import opts

class NetServer:
    connectionPool = None
    datagramDecoder = None
    serverListener = None

    #state
    _client_id = 0

    @classmethod
    def generate_client_id(cls):
        cls._client_id += 1
        return cls._client_id

    @classmethod
    def init_0(cls):
        cls.connectionPool = ConnectionPool()
        cls.datagramDecoder = DatagramDecoder()
        cls.serverListener = ServerListener()

    @classmethod
    def init_1(cls):
        cls.connectionPool.init()
        cls.datagramDecoder.init()
        cls.serverListener.init()

from chat_server import ChatServer
from net_out import SendMessage
from net_event import NetEvent
from game_state import GameStateGlobal

# listens for packets on ports
class ServerListener:

    IP = opts.ip_address
    TCP_PORT = opts.tcp_port
    UDP_PORT = opts.udp_port

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
            #self.tcp.listen(1)
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

    MAX_NAME_LENGTH = 15
    BUFFER_SIZE = 4096

    def __init__(self, connection, address):
        self.connection = connection
        self.address = address

        self.fileno = connection.fileno()
        self.TcpPacketDecoder = TcpPacketDecoder(self)
        self.sendMessage = SendMessage(self)

        self.player = None
        self.name = None
        self.ec = 0


        self._set_client_id()
        self.sendMessage.send_client_id(self) #send client an id upon connection
        self.sendMessage.game_mode()
        self.sendMessage.send_items()
        self.sendMessage.send_players() # send all players to client

    def identify(self, name):
        valid, name, you = self._valid_player_name(name)
        if valid:
            NetServer.connectionPool.name_client(self, name)
            self.name = name
            self._register()
            self.sendMessage.identified(self, 'Identified name: ' + name)
            NetOut.event.player_create(self.player)
        else:
            if you:
                self.sendMessage.identified(self, 'You were already identified as ' + name)
            else:
                self.sendMessage.identify_fail(self, 'Invalid username. ' + name)

    def send_client_id(self):
        self.sendMessage.send_client_id(self)

    def _register(self):
        ChatServer.chat.connect(self) # join chat server
        print 'Joined chat'
        if self.player is not None and self.player.id in GameStateGlobal.playerList:
            self.player.update_info(name=self.name)
            print 'Updating existing player'
        else:
            self.player = GameStateGlobal.playerList.join(self.id, self.name)  # create player
            print 'Created new player'

    def _set_client_id(self):
        if hasattr(self, 'id'):
            print "ERROR: TcpClient.set_client_id, client_id already assigned"
            return False
        self.id = str(NetServer.generate_client_id())
        return True

    def _valid_player_name(self, name):
        valid = True
        try:                                    # must be string
            name = str(name)
        except ValueError:
            name = 'Invalid client name'
            valid = False
        if not name:                           # must not be empty
            name = 'Name was empty.'
            valid = False
        if len(name) > self.MAX_NAME_LENGTH:    # truncate if longer than self.MAX_NAME_LENGTH
            name = name[0:self.MAX_NAME_LENGTH]
        avail, you = NetServer.connectionPool.name_available(name, self)
        if not avail:
            if not you:
                name = 'Name is in use.'
            valid = False
        return (valid, name, you,)

    def send(self, MESSAGE):
        try:
            if False:
                length = len(MESSAGE)
                sent = self.connection.send(MESSAGE)
                if length != sent:
                    print "ALL DATA NOT SENT!"
            else:
                '''
                http://stackoverflow.com/questions/6240737/python-socket-sendall-function
                "The function sendall() should be used only with blocking sockets."
                this is probably true
                '''
                #self.connection.sendall(MESSAGE, self.BUFFER_SIZE)
                msg_size = len(MESSAGE)
                total_sent = 0
                while total_sent < msg_size:
                    try:
                        _msg = MESSAGE[total_sent:]
                        sent = self.connection.send(_msg, self.BUFFER_SIZE)
                        total_sent += sent
                    except:
                        pass
        except socket.error, (value, message):
            print MESSAGE
            print len(MESSAGE)
            print "TcpClient.send error: " + str(value) + ", " + message
            if value == 32:  #connection reset by peer
                NetServer.connectionPool.tearDownClient(self)

    def close(self):
        print "TcpClient.close : connection closed gracefully"
        self.connection.close()

    def receive(self):
        try:
            data = self.connection.recv(self.BUFFER_SIZE)
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
        #local
        self._epoll = select.epoll()
        self._client_count = 0
        self._client_pool = {} #clients by fileno
        self._clients_by_id = {}
        self.names = {}
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
            if client.id not in self._clients_by_id:
                self._clients_by_id[client.id] = client
                print "Connection associated with client_id= %s" % (client.id,)

    def by_client_id(self, client_id):
        if client_id in self._clients_by_id:
            return self._clients_by_id[client_id]

    def name_client(self, connection, name):
        avail, you = self.name_available(name, connection)
        if not you:
            if avail:
                if connection.name in self.names:
                    del self.names[connection.name]
            else:
                return
        self.names[name] = connection.id
        return name

    def name_available(self, name, connection=None):
        you = False
        if connection is not None:
            if self.names.get(name, None) == connection.id:
                you = True
        if name in self.names:
            avail = False
        else:
            avail = True
        return (avail, you,)

    def tearDownClient(self, connection, duplicate_id = False):
        fileno = connection.fileno
        self._epoll.unregister(fileno)
        self._client_pool[fileno].close()
        del self._client_pool[fileno] #remove from client pool
        if connection.id != 0: # remove from chat
            ChatServer.chat.disconnect(connection)
            del self._clients_by_id[connection.id]
        if connection.name in self.names:
            del self.names[connection.name]
        GameStateGlobal.disconnect(connection)
        NetOut.event.client_quit(connection.id)
        ChatServer.chat.disconnect(connection)

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
        self.fmt = '<I H'
        self.fmtlen = struct.calcsize(self.fmt)

    def decode(self, message, connection):
        prefix, datagram = message[0:self.fmtlen], message[self.fmtlen:]
        length, msg_type = struct.unpack(self.fmt, prefix)
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
        fmt = '<I'
        fmtlen = struct.calcsize(fmt)
        (length,) = struct.unpack(fmt, data[0:fmtlen])
        return length

    def process_datagram(self, message):
        self.count += 1
        #print "processed message count: " +str(self.count)
        NetServer.datagramDecoder.decode(message, self.connection)

from net_out import NetOut

if __name__ == "__main__":
    print "Run run.py to start server"
