#!/usr/bin/python

from time import time as now
from copy import copy

from server_api import ServerGlobal

class ChatServerGlobal:
    chatServer = None

    def __init__(self): #first pass is declaring
        ChatServerGlobal.chat = ChatServer()
    @classmethod
    def init(self): #calls import methods if needed
        pass

'''
Chat server
'''

class Chat:

    channels = {}
    clients = {}

    def __init__(self):
        pass
#        ServerGlobal.chat = self

    # message received
    def received(self, msg, connection):
        msg = ChatMessage(msg, connection)
        self.broadcast(msg)

    # sends message to all clients subscribed to channel
    def broadcast(self, msg):
        if 'channel' not in msg:
            return
        for client_id in self.channels.get(msg['channel'], []):
            self.clients[client_id].send(msg)

    # connect client
    def connect(self, connection):
        client = self.clients.setDefault(connection.client_id, ChatClient(connection)).reconnect()

    # disconnect client (removes client if disconnected for N seconds)
    def disconnect(self, connection):
        self.remove(connection) #TEMPORARY
        return

        # use this once we have culling
        client = self.clients.get(connection.client_id, None)
        if client is not None:
            client.disconnect()

    # remove client
    def remove(self, connection):
        del self.clients[connection.client_id]

    def client_listen(self, client_id, channel=None):

        def _add_client_to_channel(client_id, channel):
            channel_parts = channel.split('_')
            if channel_parts[0] == 'pm' and channel_parts[1] != client_id:
                print 'Client attempted to listen to another\'s private client channel'
                return
            listeners = self.channels.setdefault(channel, [])
            if client_id not in listeners:
                listeners.append(client_id)

        client = self.clients.get(client_id, None)
        if client is None:
            return

        if channel is None:
            for channel in client.channels:
                _add_client_to_channel(client_id, channel)
        else:
            _add_client_to_channel(client_id, channel)


class ChatMessage:

    properties = ['cmd',
                  'client_id',
                  'content',
                  'time',
                  'channel']

    def __init__(self, msg, connection=None): # need support for system messages
        connection = connection or {}
        self.payload = copy(msg)
        self.payload['cmd'] = 'chat'
        self.payload['time'] = self.payload.get('time', None) or int(now())
        self.payload['client_id'] = connection.get('client_id', '')
        self.clean()
        self.validate()

    # removes erroneous properties
    def clean(self):
        for key in self.payload.keys():
            if key not in self.properties:
                del self.payload[key]
        self.payload['content'] = str(self.payload.get('content', ''))

    # checks if all properties are in payload
    def validate(self):
        valid = True
        for prop in self.properties:
            if prop not in self.payload:
                valid = False
        if not valid:
            self.payload = None


# an instance of a connected client
class ChatClient:

    def __init__(self, connection):
        self.connection = connection
        self.client_id = self.connection.client_id
        self.queue = []
        self.channels = ['global', 'pm_'+self.client_id]
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
