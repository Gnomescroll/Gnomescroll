#!/usr/bin/python

from time import time as now
from copy import copy
from collections import defaultdict

from server_api import ServerGlobal

class ChatServerGlobal:
    chatServer = None

    def __init__(self): #first pass is declaring
        ChatServerGlobal.chatServer = ChatServer()
    @classmethod
    def init(self): #calls import methods if needed
        pass

'''
Chat server
'''

class ChatServer:

    channels = defaultdict(list)
    clients = {}

    def __init__(self):
        self.add_channel('global')
        self.add_channel('system')

    # message received
    def received(self, msg, connection):
        print 'chatServer received msg:'
        print msg
        msg = ChatMessage(msg, connection)
        print 'ChatMessage'
        print msg.payload
        self.broadcast(msg)

    # sends message to all clients subscribed to channel
    def broadcast(self, msg):
        print msg.payload
        if msg.payload is None:
            return
        for client_id in self.channels[msg.payload['channel']]:
            print "Broadcasting %s on channel %s to client %s" % (msg.payload['content'], msg.payload['channel'], client_id,)
            self.clients[client_id].send(msg)

    # connect client
    def connect(self, connection):
        self.clients[connection.client_id] =  connection
        self.add_channel(connection.client_id)

    # disconnect client (removes client if disconnected for N seconds)
    def disconnect(self, connection):
        self.remove(connection) #TEMPORARY
        self.remove_channel(connection.client_id)
        return

        # use this once we have culling
        client = self.clients.get(connection.client_id, None)
        if client is not None:
            client.disconnect()

    # remove client
    def remove(self, connection):
        del self.clients[connection.client_id]

    def client_listen(self, client, channel=None):

        def _add_client_to_channel(client, channel):
            channel_parts = channel.split('_')
            if channel_parts[0] == 'pm' and channel_parts[1] != client.client_id:
                print 'Client attempted to listen to another\'s private client channel'
                return
            listeners = self.channels[channel]
            if client.client_id not in listeners:
                listeners.append(client.client_id)

            if channel not in client.channels:
                client.channels.append(channel)

        client = self.clients.get(client_id, None)
        if client is None:
            print 'client_listen: client unknown'
            return

        if channel is None:
            for channel in client.channels:
                _add_client_to_channel(client, channel)
        else:
            _add_client_to_channel(client, channel)

    def client_unlisten(self, client_id, channel=None):

        def _remove_client_from_channel(client, channel):
            if client.cliend_id in self.channels[channel]:
                self.channels[channel].remove(client.client_id)
            if channel in client.channels:
                client.channels.remove(channel)

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
        self.client_listen(connection.client_id, msg['channel'])
        
    def client_unsubscribe(self, msg, connection):
        self.client_unlisten(connection.client_id, msg['channel'])
        


class ChatMessage:

    properties = [
        'cmd',
        'client_id',
        'content',
        'time',
        'channel',
    ]

    def __init__(self, msg, connection=None): # need support for system messages
        connection = connection or {}
        self.payload = copy(msg)
        self.payload['cmd'] = 'chat'
        self.payload['time'] = self.payload.get('time', None) or int(now())
        self.payload['client_id'] = getattr(connection, 'client_id', '')
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
        return self


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
