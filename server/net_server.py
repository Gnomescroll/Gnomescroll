import atexit
import json
import struct

import init_c_lib
import c_lib.terrain_map as cMap

class NetServer:
    connectionPool = None

    @classmethod
    def init(cls):
        cls.connectionPool = PyClientPool()

from net_out import SendMessage
from net_event import NetEvent

from init_c_lib import connected, _send_python_net_message

class PyClient:

    def __init__(self, client_id):

        self.client_id = client_id
        self.id = client_id
        print "PyClient initialized with client_id %d" % (client_id,)

        self.sendMessage = SendMessage(self)

        self.loaded_once = False
        init_c_lib.create_agent(self.client_id)
        init_c_lib.send_id_to_client(self.client_id)
        init_c_lib.join_chat(self.client_id)

    def get_name(self):
        if not init_c_lib.client_identified(self.client_id):
            return None
        return init_c_lib.get_agent_name(self.client_id)

    def ready(self):
        if not init_c_lib.client_identified(self.client_id) or self.loaded_once:
            return
        print "Client is ready"
        self.loaded_once = True
        self.send_map()
        init_c_lib.send_game_state(self.client_id)
        
    def send_map(self):
        pass
        #print "Sending map"
        #cMap.send_map_metadata_to_client(self.client_id)
        #self.sendMessage.send_chunk_list()

    def send(self, MESSAGE):
        _send_python_net_message(MESSAGE, self.client_id)

from init_c_lib import register_client_creation, register_client_deletion, register_client_message_handling

_msg_buffer = True

class PyClientPool:

    def __init__(self):
        self.clients_by_id = {}
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
        init_c_lib.leave_chat(client_id)
        init_c_lib.leave_team(client_id)
        init_c_lib.destroy_agent(client_id)
        
        # remove from registry
        del self.clients_by_id[client.id]
        print "PyClientPool: remove_client, id= %i" % (client_id)

    def by_client_id(self, client_id):
        if client_id in self.clients_by_id:
            return self.clients_by_id[client_id]
        raise ValueError, "Unknown client_id %d", client_id

    def check_clients_ready(self):
        for client in self.clients_by_id.values():
            client.ready()

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

if __name__ == "__main__":
    print "Run run.py to start server"
