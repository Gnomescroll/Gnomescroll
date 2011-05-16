#!/usr/bin/python

'''
Chat client
'''

from time import time as now
from collections import deque

class ChatClientGlobal:
    chatClient = None

    @classmethod
    def init_0(self): #first pass is declaring
        ChatClientGlobal.chatClient = ChatClient()
    @classmethod
    def init_1(self): #calls import methods if needed
        pass

from net_client import NetClientGlobal
from net_out import NetOut
from net_event import NetEventGlobal

'''
To send messages, use
NetOut.chatMessage.send_chat(d)

Modify chatMessage classs in net_out.py for additional message types
client_id is NetClientGlobal.client_id

Incoming json message handler is in net_event.py
'''

CONFIG_PATH = './conf'

class ChatClient:

    CURRENT_CHANNEL = 'global'
    ignored = []
    subscriptions = {}

    _conf = 'chat.conf'

    def __init__(self, channel=None):
        channel is not None or self.set_current_channel(channel)
        self.subscribe('system')
        self.load()

    def set_current_channel(self, channel):
        if self.CURRENT_CHANNEL != channel:
            self.unsubscribe(self.CURRENT_CHANNEL)
        self.CURRENT_CHANNEL = channel
        self.subscribe(self.CURRENT_CHANNEL)

    def subscribe(self, channel):
        assert type(channel) == str
        if channel == 'system':
            self.subscriptions.setdefault(channel, SystemChannel(channel))
        else:
            self.subscriptions.setdefault(channel, Channel(channel))
        print 'Chat client subscribed to %s' % (channel,)

    def unsubscribe(self, channel):
        if channel == 'system':
            return
        del self.subscriptions[channel]

    # add client_id to ignore list
    def ignore(self, client_id):
        client_id in self.ignored or self.ignored.append(client_id)

    # remove client_id from ignore list
    def unignore(self, client_id):
        client_id in self.ignored and self.ignored.remove(client_id)

    def send(self, text):
        if not NetClientGlobal.client_id:
            print 'Client_id is not set; cannot send chat msg'
            return

        if text[0] == '/':
            msg = ChatCommand(text).send()
        else:
            msg = ChatMessageOut(text).send()

    # receive incoming message
    def receive(self, msg):
        if channel in subscriptions:
            msg = ChatMessageIn(msg)
            if msg.payload.client_id in self.ignored:
                return
            subscriptions[channel].receive(msg)

    # saves ignored list + subscription channel names
    def save(self):
        from simplejson import dumps as encode_json
        from os import mkdir
        from os.path import exists as path_exists

        path_exists(CONFIG_PATH) or mkdir(CONFIG_PATH)
        with open(CONFIG_PATH + '/' + self._conf, 'w') as f:
            f.write(encode_json({
                'ignored': self.ignored,
                'subscriptions' : self.subscriptions.keys(),
            }))

    # loads saved ignore list & subscription channels
    def load(self):
        from simplejson import loads as decode_json
        from os.path import exists as path_exists
        
        conf = CONFIG_PATH + '/' + self._conf
        if path_exists(conf):
            with open(conf, 'r') as f:
                settings = decode_json(f.read())
                self.ignored = settings.get('ignored', None) or self.ignored
                for channel in settings.get('subscriptions', []):
                    self.subscribe(channel)
            

# channel wrapper
class Channel:

    HISTORY_MAX = 200

    def __init__(self, name):
        self.name = name
        self.history = deque([], self.HISTORY_MAX)

    def receive(self, msg):
        if msg.valid:
            self.history.appendleft(msg)

# special channel for system msgs (has extra processing specific to /commands)
class SystemChannel(Channel):

    def receive(self, msg):
        log = None
        if msg.content == 'ping':
            log = Payload({
                'content'  : 'Chat ping round-trip time: ' + (int(now()) - int(msg.time)),
                'channel'  : 'system',
                'client_id': 'system',
                'cmd'      : 'chat',
            })

        if log is not None:
            self.history.appendleft(log)

# command message (e.g. /channel highlands i am messaging the highlands channel)
class ChatCommand():

    def __init__(self, text):
        text_pts = text.split(' ')
        command = text_pts[0][1:]
        args = text_pts[1:]
        self.route(command, args)
        self._send = None
        self.payload = None

    # create a special payload and/or a special _send command
    def route(self, command, args):
        _send = None
        payload = None
        
        if command == 'channel':
            payload = Payload({
                'content'  : str(' '.join(args[1:])),
                'channel'  : args[0]
            })

        elif command == 'version':
            _send = self._send_local({
                'content' : 'DCMMO Client version: ' + NetClientGlobal.VERSION,
                'channel' : 'system'
            })

        elif command == 'ping':
            payload = Payload({
                'channel' : 'system',
                'content' : 'ping'
            })

        elif command == 'save':
            _send = lambda: ChatClientGlobal.chatClient.save()

        else:
            _send = self._send_local({
                'content' : command + ' command is not implemented.',
                'channel' : 'system'
            })

        self.payload = payload
        self._send = _send

    def _send_local(self, data):
        def _send():
            ChatClientGlobal.chatClient.receive(Payload(data).serialize())
        return _send
        
    def send(self):
        if self._send is not None:
            self._send()
        else:
            if self.payload is not None:
                NetOut.chatMessage.send_chat(self.payload.serialize())

# msg to be sent
class ChatMessageOut():

    def __init__(self, text):
        self.payload = Payload({
            'content' : str(text),
            'channel' : ChatClient.CURRENT_CHANNEL,
        })
        self.payload.clean()
        self.valid = self.payload.valid()

    def send(self):
        NetOut.chatMessage.send_chat(self.payload.serialize()) # fix this reference
        print 'Sent chat message'

# msg received
class ChatMessageIn():

    def __init__(self, msg):
        self.payload = Payload(msg)
        self.payload.clean()
        self.valid = self.payload.valid()
        self.timestamp = int(now())

# msg payload, attached to a ChatMessageIn/Out or (optionally) ChatCommand
class Payload:

    properties = [
        'cmd',
        'content',
        'time',
        'channel',
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
        

if __name__ == '__main__':
    ChatClientGlobal.init_0()
    ChatClientGlobal.init_1()
