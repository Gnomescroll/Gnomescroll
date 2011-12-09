import opts
opts = opts.opts

import atexit
import select
import socket
import json
import struct
import platform

import init_c_lib


#OS = platform.system()
#OS = "Windows"

class NetServer:
    connectionPool = None
    #datagramDecoder = None
    #serverListener = None

    #state
    #_client_id = 0

    #@classmethod
    #def generate_client_id(cls):
    #    cls._client_id += 1
    #    return cls._client_id

    @classmethod
    def init_0(cls):
        global OS
        cls.connectionPool = PyClientPool()

        #cls.datagramDecoder = DatagramDecoder()
        #cls.serverListener = ServerListener()

    @classmethod
    def init_1(cls):
        cls.connectionPool.init()
        #cls.datagramDecoder.init()
        #cls.serverListener.init()

from chat_server import ChatServer
from net_out import SendMessage
from net_event import NetEvent
from game_state import GameStateGlobal

#for creation of client
#NetServer.connectionPool.addClient(connection, address) #hand off connection to connection pool

# manages TCP stuff and is somehow different from ServerListener and TcpPacketDecoder

from init_c_lib import connected, _send_python_net_message

class PyClient:

    MAX_NAME_LENGTH = 15
    #BUFFER_SIZE = 4096

    def __init__(self, client_id):

        self.client_id = client_id
        #self.connection = connection
        #self.address = address

        self.admin = False

        self.received_id = False
        self.identified = False
        self.dat_loaded = False
        self.loaded_once = False

        #self.fileno = connection.fileno()
        #self.TcpPacketDecoder = TcpPacketDecoder(self)
        self.sendMessage = SendMessage(self)

        self.player = None
        self.name = None
        #self.ec = 0

        #self._set_client_id()
        self.sendMessage.send_client_id(self) #send client an id upon connection
        self.sendMessage.send_dat()

    def identify(self, name):
        valid, name, you = self._valid_player_name(name)
        if valid:
            self.identified = True
            print 'IDENTIFIED'
            NetServer.connectionPool.name_client(self, name)
            self.name = name
            self.check_ready()
        else:
            if you:
                self.sendMessage.identified(self, 'You were already identified as %s' % (name,))
            else:
                self.sendMessage.identify_fail(self, 'Invalid username. %s' % (name,))

    def check_ready(self):
        if self.dat_loaded and self.identified:
            self.ready()
            return True
        return False

    def ready(self):
        if self.loaded_once:
            return
        self.loaded_once = True
        print '%s ready' % (self.id,)
        self._register()
        self.start_player()
        self.send_game_state()

    def set_dat_loaded(self):
        self.dat_loaded = True
        self.check_ready()

    #id is automatic, send dat afterwards
    #def set_id_received(self):
    #    if not self.received_id:
    #        self.received_id = True
    #        self.sendMessage.send_dat()

    def send_client_id(self):
        self.sendMessage.send_client_id(self)

    def start_player(self):
        NetOut.event.player_create(self.player)

    def send_game_state(self):
        self.sendMessage.send_players()
        self.sendMessage.game_mode()
        self.sendMessage.send_items()

    def _register(self):
        ChatServer.chat.connect(self) # join chat server
        print 'Joined chat'
        if self.player is not None and self.player.id in GameStateGlobal.playerList:
            self.player.update_info(name=self.name)
            print 'Updating existing player'
        else:
            self.player = GameStateGlobal.playerList.join(self, self.name)  # create player
            print 'Created new player'
        self.sendMessage.identified(self, 'Identified name: %s' % (self.name,))

    '''
    def _set_client_id(self):
        if hasattr(self, 'id'):
            print "ERROR: TcpClient.set_client_id, client_id already assigned"
            return False
        self.id = str(NetServer.generate_client_id())
        return True
    '''

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
        _send_python_net_message(MESSAGE, self.client_id)

from init_c_lib import register_client_creation, register_client_deletion, register_client_message_handling


_msg_buffer = True



#function = lambda: 42
class PyClientPool:

    def init(self):
        pass
    def __init__(self):
        self._client_count = 0
        self.clients_by_id = {}
        self.names = {}
        atexit.register(self.on_exit)
    
        
        global _msg_buffer     
        if _msg_buffer: 
            self.event_buffer = []
            self.message_buffer = []
            register_client_message_handling(( lambda _client_id, message : self.push_to_buffer(_client_id, message) ))
            register_client_creation(( lambda _client_id: self.push_to_event_buffer(_client_id,0) ))
            register_client_deletion(( lambda _client_id: self.push_to_event_buffer(_client_id,1) ))

        else:
            register_client_message_handling(( lambda _client_id, message : self.handleMessage(_client_id, message) ))
            register_client_creation(( lambda _client_id: self.addClient(_client_id) ))
            register_client_deletion(( lambda _client_id: self.removeClient(_client_id) ))
        #for messages
        self.fmt = '<H'
        self.fmtlen = struct.calcsize(self.fmt)

    def push_to_event_buffer(self, client_id, event_id):
        self.event_buffer.append((client_id, event_id))
    def dispatch_event_buffer(self):
        for client_id, event_id in self.event_buffer:
            if event_id == 0:
                self.addClient(client_id)
            if event_id == 1:
                self.removeClient(client_id)
        self.event_buffer = []

    def push_to_buffer(self, client_id, message):
        self.message_buffer.append([client_id, message])
    def dispatch_buffer(self):
        global _msg_buffer     
        if not _msg_buffer:
            return
        self.dispatch_event_buffer()
        for client_id, message in self.message_buffer:
            self.handleMessage(client_id, message)
        self.message_buffer = []

    def handleMessage(self, client_id, message):
        #message handling
        msg_type = struct.unpack(self.fmt, message[0:2])
        msg_type = msg_type[0]
        message = message[2:]
        print "client %i: %s" % (client_id, message)
        #print "length=%i, msg=%s" % (length, message)
        connection = self.clients_by_id.get(client_id, None)
        if connection == None:
            print "PyClientPool: handleMessage, client id does not exist in pool"
            return
        if msg_type == 0:
            print "test message received"
        elif msg_type == 1: #client json messages
            try:
                msg = json.loads(datagram)
            except Exception, e:
                print Exception, e
                print "JSON DECODING ERROR: %s" % (str(datagram),)
                return
            NetEvent.messageHandler.process_json(msg, connection)
        elif msg_type == 2: #client admin messages
            try:
                msg = json.loads(datagram)
            except:
                print "JSON DECODING ERROR: %s" % (str(datagram),)
                return
            NetEvent.adminMessageHandler.process_json(msg, connection)

    def on_exit(self):
        pass

    #called when client connection established
    def addClient(self, _client_id):
        print "PyClientPool: addClient, id= %i" % (_client_id)
        self._client_count += 1
        client =  PyClient(_client_id)
        self.clients_by_id[_client_id] = client
        print "Connection associated with client_id= %s" % (_client_id)

    #called on connection deconstruction
    def removeClient(self, _client_id):
        print "PyClientPool: removeClient, id= %i" % (_client_id)
        del self.clients_by_id[client.id]

    def by_client_id(self, client_id):
        if client_id in self.clients_by_id:
            return self.clients_by_id[client_id]

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

    '''
    def tearDownClient(self, connection, duplicate_id = False):
        fileno = connection.fileno
        for sock in self.socket_list:
            if sock.fileno() == fileno:
                self.socket_list.remove(sock)
        self._client_pool[fileno].close()
        del self._client_pool[fileno] #remove from client pool
        if connection.id != 0: # remove from chat
            ChatServer.chat.disconnect(connection)
            del self.clients_by_id[connection.id]
        if connection.name in self.names:
            del self.names[connection.name]
        GameStateGlobal.disconnect(connection)
        NetOut.event.client_quit(connection.id)
        ChatServer.chat.disconnect(connection)
    '''

'''
Decoders
'''

# decodes datagram, passes to messageHandler
class DatagramDecoder:

    def init(self):
        pass
    def __init__(self):
        #self.fmt = '<I H'
        self.fmt = '<H'
        self.fmtlen = struct.calcsize(self.fmt)

    #packet processing entry point
    def decode(self, message, connection):
        #prefix, datagram = message[0:self.fmtlen], message[self.fmtlen:]
        length, msg_type = struct.unpack(self.fmt, prefix)
        if msg_type == 0:
            print "test message received"
        elif msg_type == 1: #client json messages
            try:
                msg = json.loads(datagram)
            except Exception, e:
                print Exception, e
                print "JSON DECODING ERROR: %s" % (str(datagram),)
                return
            NetEvent.messageHandler.process_json(msg, connection)
        elif msg_type == 2: #client admin messages
            try:
                msg = json.loads(datagram)
            except:
                print "JSON DECODING ERROR: %s" % (str(datagram),)
                return
            NetEvent.adminMessageHandler.process_json(msg, connection)

# decodes tcp packets
'''
class TcpPacketDecoder:

    def __init__(self, connection):
        self.connection = connection
        self.reset()

    def process_datagram(self, message):
        NetServer.datagramDecoder.decode(message, self.connection)
'''

from net_out import NetOut

if __name__ == "__main__":
    print "Run run.py to start server"
