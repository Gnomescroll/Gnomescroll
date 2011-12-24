#!/usr/bin/python

from collections import defaultdict
from net_server import NetServer
from utils import now

class ChatServer:
    chat = None
    _msg_id = 0
    
    def __init__(self): #first pass is declaring
        ChatServer.chat = Chat()

    @classmethod
    def generate_message_id(cls):
        cls._msg_id += 1
        return cls._msg_id

'''
Chat server
'''

class Chat:

    channels = defaultdict(list)
    clients = {}

    def __init__(self):
        self.add_channel('global')
        self.add_channel('system')

    # message received
    def received(self, msg, connection):
        print 'chatServer received msg:'
        print msg
        if connection.name is None:
            print 'Client %s unidentified, discarding msg' % (connection.id,)
            return
        msg = ChatMessage(msg, connection)
        self.broadcast(msg)

    # sends message to all clients subscribed to channel
    def broadcast(self, msg, private=None):
        if msg.payload is None:
            return
        print 'ChatMessage broadcast'
        print msg.payload
        if 'channel' not in msg.payload:
            return
        print self.channels[msg.payload['channel']]
        for client_id in self.channels[msg.payload['channel']]:
            print "Broadcasting %s on channel %s to client %d" % (msg.payload['content'], msg.payload['channel'], client_id,)
            self.clients[client_id].send(msg)

    # connect client
    def connect(self, connection):
        client = ChatClient(connection)
        self.clients[client.id] = client
        print self.clients
        self.add_channel('pm_'+str(client.id))
        print "client %d connected to chat" % (client.id,)

    # disconnect client
    def disconnect(self, connection):
        if connection.id not in self.clients:
            return
        client = self.clients[connection.id]
        self.remove(client) 
        self.remove_channel('pm_' + str(client.id))
        print 'chat disconnected client'

    # remove client
    def remove(self, client):
        for channel in client.channels:
            if channel in self.channels and client.id in self.channels[channel]:
                self.channels[channel].remove(client.id)
        del self.clients[client.id]
        print 'chat removed client'

    def client_listen(self, connection, channel=None):

        def _add_client_to_channel(client, channel):
            channel_parts = channel.split('_')
            if channel_parts[0] == 'pm' and channel_parts[1] != str(client.id):
                print 'Client attempted to listen to another\'s private client channel'
                return
            listeners = self.channels[channel]
            if client.id not in listeners:
                print 'client %d now listening to %s' %(client.id, channel,)
                listeners.append(client.id)

            if channel not in client.channels:
                client.channels.append(channel)

        client = self.clients.get(connection.id, None)
        if client is None:
            print 'client_listen: client unknown'
            return

        if channel is None:
            for channel in client.channels:
                _add_client_to_channel(client, channel)
        else:
            _add_client_to_channel(client, channel)

    def client_unlisten(self, connection, channel=None):

        def _remove_client_from_channel(client, channel):
            client_id = str(client.id)
            listeners = self.channels[channel]
            if client_id in listeners:
                listeners.remove(client_id)
            if channel in client.channels:
                client.channels.remove(channel)

        client_id = connection.id
        client = self.clients.get(client_id, None)
        if client is None:
            print 'client_unlisten: client unknown'
            return

        if channel is None:
            for channel in client.channels:
                _remove_client_from_channel(client, channel)
        else:
            _remove_client_from_channel(client, channel)

    def add_channel(self, channel):
        if channel not in self.channels:
            self.channels[channel] = []

    def remove_channel(self, channel):
        if channel in self.channels:
            del self.channels[channel]

    def client_subscribe(self, msg, connection):
        print 'client subscribing to: %s' % (msg['channel'],)
        self.client_listen(connection, msg['channel'])

    def client_unsubscribe(self, msg, connection):
        print 'client unsubscribing to: %s' % (msg['channel'],)
        self.client_unlisten(connection, msg['channel'])


class ChatMessage:

    properties = [
        'cmd',
        'cid',
        'content',
        'time',
        'channel',
        'id',
    ]

    def __init__(self, msg, connection=None): # need support for system messages
        connection = connection or {}
        self.payload = {}
        self.payload['cmd'] = 'chat'
        self.payload['time'] = msg.get('time', None) or int(now())
        self.payload['cid'] = getattr(connection, 'id', '')
        self.payload['content'] = msg.get('content', '')
        self.payload['channel'] = msg.get('channel', '')
        self.payload['id'] = ChatServer.generate_message_id()
        self.validate()
            
    # checks if all properties are in payload
    def validate(self):
        valid = True
        if not self.payload['content']:
            valid = False
        if not self.payload['channel']:
            valid = False
        if not valid:
            self.payload = None
        return self

# an instance of a connected client
class ChatClient:

    def __init__(self, connection):
        self.connection = connection
        self.id = self.connection.id
        self.queue = []
        self.channels = ['global', 'pm_'+str(self.id)]
        self.connected = True

    def flush_queue(self):
        for msg in self.queue:
            self.sendMessage.send_json(msg.payload)

    def reconnect(self):
        self.connected = True
        self.flush_queue()

    def disconnect(self):
        self.connected = False

    def listen(self, channel):
        if channel not in self.channels:
            self.channels.append(channel)

    def send(self, msg):
        if self.connected:
            self.connection.sendMessage.send_json(msg.payload)
        else:
            self.queue.append(msg)
