import opts
opts = opts.opts

import atexit
import select
import socket
import json
import struct
import platform

import init_c_lib
import c_lib.c_lib_agents as cAgents
import c_lib.c_lib_monsters as cMonsters
import c_lib.c_lib_game_modes as cGameModes

class NetServer:
    connectionPool = None

    @classmethod
    def init(cls):
        cls.connectionPool = PyClientPool()

from chat_server import ChatServer
from net_out import SendMessage
from net_event import NetEvent
from game_state import GameStateGlobal

#for creation of client
#NetServer.connectionPool.add_client(connection, address) #hand off connection to connection pool

# manages TCP stuff and is somehow different from ServerListener and TcpPacketDecoder

from init_c_lib import connected, _send_python_net_message

class PyClient:

    MAX_NAME_LENGTH = 15

    def __init__(self, client_id):

        self.client_id = client_id
        self.id = client_id
        print "PyClient initialized with client_id %d" % (client_id,)

        self.admin = False

        self.identified = False
        self.dat_loaded = False
        self.loaded_once = False

        self.sendMessage = SendMessage(self)

        self.agent = None
        self.name = None

        self.sendMessage.send_dat()

    def identify(self, name):
        valid, name, you = self.valid_player_name(name)
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
        if self.loaded_once:
            return
        print "Client is ready"
        self.loaded_once = True
        self._register()
        self.send_map()
        self.send_agents()
        self.send_monsters()
        self.send_ctf()
        self.send_name_to_clients()

    def send_agents(self):
        cAgents.AgentListWrapper.send_to_client(self.client_id)

    def send_monsters(self):
        cMonsters.send_to_client(self.client_id)

    def send_ctf(self):
        cGameModes.send_ctf_to_client(self.client_id)

    def send_name_to_clients(self):
        for client in NetServer.connectionPool.clients_by_id.values():
            if client.id == self.client_id: continue
            client.sendMessage.send_agent_name(self.agent.id, self.name)

    def send_map(self):
        print "Sending map"
        self.sendMessage.send_chunk_list()

    def set_dat_loaded(self):
        print "Client loaded dat"
        self.dat_loaded = True
        self.check_ready()

    def _register(self):
        ChatServer.chat.connect(self) # join chat server
        if self.agent is not None and self.agent.id in GameStateGlobal.agentList:
            self.agent.update_info(name=self.name)
        else:
            self.agent = GameStateGlobal.agentList.create(self.client_id)
            cAgents.set_agent_name(self.agent.id, self.name)
            print 'Created new agent'
            # send player agent id
            self.agent.send_id_to_client(self.client_id)
            print "Send playerAgent id"
        self.sendMessage.identified(self, 'Identified name: %s' % (self.name,))

    def valid_player_name(self, name):
        valid = True
        try:                                    # must be string
            name = str(name)
        except ValueError:
            name = 'Invalid client name'
            valid = False
        if not name:                           # must not be empty
            name = 'Name was empty.'
            valid = False
        name = name[:self.MAX_NAME_LENGTH] #truncate
        avail, you = NetServer.connectionPool.name_available(name, self)
        if not avail:
            if not you:
                name = 'Name is in use.'
            valid = False
        return (valid, name, you,)

    _i = 0
    def send(self, MESSAGE):
        #if self._i > 700 and 20 < len(MESSAGE) < 50:
            #raise Exception
        #self._i += 1
        _send_python_net_message(MESSAGE, self.client_id)

from init_c_lib import register_client_creation, register_client_deletion, register_client_message_handling

_msg_buffer = True

class PyClientPool:

    def __init__(self):
        self.clients_by_id = {}
        self.names = {}
        atexit.register(lambda: None)
        
        global _msg_buffer     
        if _msg_buffer: 
            self.event_buffer = []
            self.message_buffer = []
            register_client_message_handling(( lambda _client_id, message : self.push_to_buffer(_client_id, message) ))
            register_client_creation(( lambda _client_id: self.push_to_event_buffer(_client_id,0) ))
            register_client_deletion(( lambda _client_id: self.push_to_event_buffer(_client_id,1) ))

        else:
            register_client_message_handling(( lambda _client_id, message : self.handleMessage(_client_id, message) ))
            register_client_creation(( lambda _client_id: self.add_client(_client_id) ))
            register_client_deletion(( lambda _client_id: self.remove_client(_client_id) ))
        #for messages
        self.fmt = '<H'
        self.fmtlen = struct.calcsize(self.fmt)

    def push_to_event_buffer(self, client_id, event_id):
        self.event_buffer.append((client_id, event_id))
    def dispatch_event_buffer(self):
        for client_id, event_id in self.event_buffer:
            if event_id == 0:
                self.add_client(client_id)
            if event_id == 1:
                self.remove_client(client_id)
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
        connection = self.clients_by_id.get(client_id, None)
        if connection == None:
            print "PyClientPool: handleMessage, client id %d does not exist in pool" % (client_id,)
            return
        if msg_type == 0:
            print "test message received"
        elif msg_type == 1: #client json messages
            try:
                msg = json.loads(message)
                #print msg['cmd']
            except Exception, e:
                print Exception, e
                print "JSON DECODING ERROR: %s" % (str(message),)
                return
            NetEvent.messageHandler.process_json(msg, connection)
        elif msg_type == 2: #client admin messages
            try:
                msg = json.loads(message)
                #print msg['cmd']
            except:
                print "JSON DECODING ERROR: %s" % (str(message),)
                return
            NetEvent.adminMessageHandler.process_json(msg, connection)

    #called when client connection established
    def add_client(self, _client_id):
        print "python add_client callback: client_id %d" % (_client_id,)
        client =  PyClient(_client_id)
        self.clients_by_id[_client_id] = client
        print "PyClientPool: connection associated with client_id= %s" % (_client_id)

    #called on connection deconstruction
    def remove_client(self, client_id):
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
        print "PyClientPool: remove_client, id= %i" % (client_id)

    def by_client_id(self, client_id):
        if client_id in self.clients_by_id:
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

    def __init__(self):
        self.fmt = '<H'
        self.fmtlen = struct.calcsize(self.fmt)

    #packet processing entry point
    def decode(self, message, connection):
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

from net_out import NetOut

if __name__ == "__main__":
    print "Run run.py to start server"
