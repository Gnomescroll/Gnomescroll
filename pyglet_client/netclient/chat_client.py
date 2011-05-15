#!/usr/bin/python

from time import time as now
from collections import deque

from client_api import ClientGlobal

'''
Chat client
'''

class Chat:

    CURRENT_CHANNEL = 'global'
    ignored = []
    subscriptions = {}
    
    def __init__(self, sendMessage, channel=None):
        self.sendMessage = sendMessage
        channel is not None or self.set_CURRENT_CHANNEL(channel)
        self.subscribe('system')
        
    def set_current_channel(self, channel):
        channel = str(channel)
        if self.CURRENT_CHANNEL != channel:
            self.unsubscribe(self.CURRENT_CHANNEL)
        self.CURRENT_CHANNEL = channel
        self.subscribe(self.CURRENT_CHANNEL)

    def subscribe(self, channel):
        self.subscriptions.setdefault(channel, Channel(channel))

    def unsubscribe(self, channel):
        del self.subscriptions[channel]

    def ignore(self, client_id):
        client_id in self.ignored or self.ignored.append(client_id)

    def unignore(self, client_id):
        client_id in self.ignored and self.ignored.remove(client_id)

    def send(self, text):
        if not ClientGlobal.client_id:
            print 'Client_id is not set; cannot send chat msg'
            return

        if text[0] == '/':
            msg = ChatCommand(text).send()
        else:
            msg = ChatMessageOut(text).send()

    def receive(self, msg):
        if msg.client_id in self.ignored:
            return
        if channel in subscriptions:
            msg = ChatMessageIn(msg)
            subscriptions[channel].receive(msg)

# channel wrapper
class Channel:

    def __init__(self, name):
        self.name = name
        self.history = deque([], 200)

    def receive(self, msg)
        if msg.valid:
            self.history.appendleft(msg)

# command message (e.g. /channel highlands i am messaging the highlands channel)
class ChatCommand():

    def __init__(self, text):
        text_pts = text.split(' ')
        command = text_pts[0][1:]
        args = text_pts[1:]
        self.route(command, args)

    # create a special payload and/or a special _send command
    def route(self, command, args):
        if command == 'channel':
            self.payload = Payload({
                'content'  : str(' '.join(args[1:])),
                'channel'  : args[0]
            })
        elif command == 'version':
            def _send():
                ClientGlobal.chat.receive(Payload({
                    'content' : 'DCMMO Client version: ' + ClientGlobal.VERSION,
                    'channel' : 'system'
                }).serialize())
            self._send = _send
        else:
            self.payload = None
            

    def send(self):
        if getattr(self, '_send', None) is not None:
            self._send()
        else:
            if self.payload is not None:
                ClientGlobal.sendMessage.send_chat(self.payload.serialize())

# msg to be sent
class ChatMessageOut():

    def __init__(self, text):
        self.payload = Payload({
            'content' : str(text),
            'channel' : Chat.CURRENT_CHANNEL
        })
        self.payload.clean()
        self.valid = self.payload.valid()

    def send(self):
        ClientGlobal.sendMessage.send_chat(self.payload.serialize()) # fix this reference
        print 'Sent chat message'

# msg received
class ChatMessageIn():

    def __init__(self, msg):
        self.payload = Payload(msg)
        self.payload.clean()
        self.valid = self.payload.valid()

# msg payload, attached to a ChatMessageIn/Out or (optionally) ChatCommand
class Payload:

    properties = [
        'cmd',
        'content',
        'time',
        'channel'
    ]

    def __init__(self, **kwargs):
        self.cmd = kwargs.get('cmd', 'chat')
        self.content = kwargs.get('content', '')
        self.time = kwargs.get('time', int(now()))
        self.channel = kwargs.get('channel', '')
        self.valid()

    # checks if all properties are in payload
    def valid(self, properties=None):
        properties = properties or self.properties
        valid = True
        for prop in properties:
            if getattr(self, p, None) is None:
                valid = False
        self.valid = valid
        return self.valid

    def serialize(self):
        d = {}
        for p in self.properties:
            d[p] = getattr(self, p, None)
        return d
