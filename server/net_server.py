import opts
opts = opts.opts

import atexit
import select
import socket
import json
import struct
import platform

import init_c_lib

class NetServer:
    connectionPool = None

    @classmethod
    def init_0(cls):
        global OS
        cls.connectionPool = PyClientPool()

    @classmethod
    def init_1(cls):
        cls.connectionPool.init()

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
        self.id = client_id
        print "PyClient initialized with client_id %d" % (client_id,)

        #self.connection = connection
        #self.address = address

        self.admin = False

        #self.received_id = False
        self.identified = False
        self.dat_loaded = False
        self.loaded_once = False

        #self.fileno = connection.fileno()
        #self.TcpPacketDecoder = TcpPacketDecoder(self)
        self.sendMessage = SendMessage(self)

        #self.player = None
        self.agent = None
        self.name = None
        #self.ec = 0

        #self.sendMessage.send_client_id(self) #send client an id upon connection
        self.sendMessage.send_dat()

    def identify(self, name):
        print "Client identifying as %s" % (name,)
        valid, name, you = self._valid_player_name(name)
        print valid, name, you
        if valid:
            self.identified = True
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
        print "Client is ready"
        if self.loaded_once:
            return
        self.loaded_once = True
        self._register()
        self.send_agents()  # temporary, really only for weapons
        self.send_map()

    def send_agents(self):
        self.sendMessage.send_agents()

    def send_map(self):
        self.sendMessage.send_chunk_list()

    def set_dat_loaded(self):
        print "Client loaded dat"
        self.dat_loaded = True
        self.check_ready()

    def _register(self):
        ChatServer.chat.connect(self) # join chat server
        #if self.player is not None and self.player.id in GameStateGlobal.playerList:
        if self.agent is not None and self.agent.id in GameStateGlobal.agentList:
            #self.player.update_info(name=self.name)
            self.agent.update_info(name=self.name)
        else:
            #self.player = GameStateGlobal.playerList.join(self, self.name)  # create player
            #self.player = GameStateGlobal.playerList.join(self)  # create player
            #self.agent = GameStateGlobal.agentList.join(self)  # create player
            self.agent = GameStateGlobal.agentList.create(self.client_id)
            print 'Created new agent'
        self.sendMessage.identified(self, 'Identified name: %s' % (self.name,))

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
        #print self.event_buffer
        for client_id, event_id in self.event_buffer:
            if event_id == 0:
                #assert False
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
        #print "client %i: %s" % (client_id, message)
        #print "length=%i, msg=%s" % (length, message)
        connection = self.clients_by_id.get(client_id, None)
        if connection == None:
            print "PyClientPool: handleMessage, client id does not exist in pool"
            return
        if msg_type == 0:
            print "test message received"
        elif msg_type == 1: #client json messages
            try:
                msg = json.loads(message)
            except Exception, e:
                print Exception, e
                print "JSON DECODING ERROR: %s" % (str(message),)
                return
            NetEvent.messageHandler.process_json(msg, connection)
        elif msg_type == 2: #client admin messages
            try:
                msg = json.loads(message)
            except:
                print "JSON DECODING ERROR: %s" % (str(message),)
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
        #self._client_pool[_client_id] = client #save client
        print "Connection associated with client_id= %s" % (_client_id)

    #called on connection deconstruction
    def removeClient(self, client_id):
        print "PyClientPool: removeClient, id= %i" % (client_id)
        client = self.clients_by_id[client_id]

        # dispatch event
        ChatServer.chat.disconnect(client)
        GameStateGlobal.disconnect(client)
        NetOut.event.client_quit(client.id)

        # recycle name
        if client.name in self.names:
            del self.names[client.name]

        # remove from registry
        del self.clients_by_id[client.id]

    def by_client_id(self, client_id):
        if client_id in self.clients_by_id:
            print self.clients_by_id
            return self.clients_by_id[client_id]
        raise ValueError, "Unknown client_id %d", client_id

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

    def name_available(self, name, connection):
        you = False
        avail = True
        if name in self.names:
            if self.names[name] == connection.id:
                you = True
            else:
                avail = False
        return (avail, you,)

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
