#!/usr/bin/python

'''
Chat client
'''

from time import time as now
from collections import deque
from string import lowercase, uppercase, letters, digits

class ChatClientGlobal:
    chatClient = None

    @classmethod
    def init_0(self): #first pass is declaring
        ChatClientGlobal.chatClient = ChatClient()
        ChatClientGlobal.chatRender = ChatRender()
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

CONFIG_PATH = './conf' # move to a client global class

class ChatClient:

    CURRENT_CHANNEL = 'global'
    ignored = []
    subscriptions = {}

    _conf = 'chat.conf'

    def __init__(self, channel=None):
        self.input = ChatInput()
        if channel is not None:
            self.set_current_channel(channel)
        self.subscribe(self.CURRENT_CHANNEL)
        self.subscribe('system')
        self.load()

    def set_current_channel(self, channel):
        if self.CURRENT_CHANNEL != channel:
            self.unsubscribe(self.CURRENT_CHANNEL)
        self.CURRENT_CHANNEL = channel
        self.subscribe(self.CURRENT_CHANNEL)

    def subscribe(self, channel):
        assert type(channel) == str
        if not channel:
            return
        if channel == 'system':
            self.subscriptions.setdefault(channel, SystemChannel(channel))
        else:
            self.subscriptions.setdefault(channel, Channel(channel))
        print 'Chat client subscribed to %s' % (channel,)

    def unsubscribe(self, channel):
        if channel == 'system':
            return
        if channel in self.subscriptions:
            del self.subscriptions[channel]

    # add client_id to ignore list
    def ignore(self, client_id):
        if client_id not in self.ignored:
            self.ignored.append(client_id)

    # remove client_id from ignore list
    def unignore(self, client_id):
        if client_id in self.ignored:
            self.ignored.remove(client_id)

    def send(self, text=None):
        if not NetClientGlobal.client_id:
            print 'Client_id is not set; cannot send chat msg'
            return

        if text is None:
            text = self.input.submit()
        if text[0] == '/':
            msg = ChatCommand(text)
        else:
            msg = ChatMessageOut(text)
        msg.send()

    # receive incoming message
    def receive(self, msg):
        print 'Received msg:'
        print msg
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
                ignored = settings.get('ignored', None)
                if ignored is not None:
                    self.ignored = ignored
                for channel in settings.get('subscriptions', []):
                    self.subscribe(channel)
            

# channel wrapper
class Channel:

    HISTORY_MAX = 200

    def __init__(self, name):
        self.name = name
        self.history = deque([], self.HISTORY_MAX)
        self._curr_iter = 0
    
    def next(self):
        if self._curr_iter == len(self.history):
            raise StopIteration
        else:
            self._curr_iter += 1
            return self.history[self._curr_iter-1]
        
    def __iter__(self):
        return self

    def receive(self, msg):
        if msg.valid:
            self.history.appendleft(msg)

# special channel for system msgs (has extra processing specific to /commands)
class SystemChannel(Channel):

    def receive(self, msg):
        log = None
        if msg.content == 'ping':
            log = Payload(
                content   = 'Chat ping round-trip time = ' + (int(now()) - int(msg.time)),
                channel   = 'system',
                client_id = 'system',
                cmd       = 'chat',
            )

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
            payload = Payload(
                content = str(' '.join(args[1:])),
                channel = args[0]
            )

        elif command == 'version':
            _send = self._send_local({
                'content' : 'DCMMO Client version: ' + NetClientGlobal.VERSION,
                'channel' : 'system'
            })

        elif command == 'ping':
            payload = Payload(
                channel = 'system',
                content = 'ping'
            )

        elif command == 'save':
            _send = lambda: ChatClientGlobal.chatClient.save()

        elif command == 'join':
            _send = lambda: ChatClientGlobal.chatClient.set_current_channel(args[0])

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
        print 'Sending msg: "%s" to: %s' % (str(text), ChatClientGlobal.chatClient.CURRENT_CHANNEL,)
        self.payload = Payload(
            content = str(text),
            channel = ChatClientGlobal.chatClient.CURRENT_CHANNEL,
        )
        self.payload.clean()
        self.valid = self.payload.valid()

    def send(self):
        if self.payload is None or not self.payload.content:
            return False
        NetOut.chatMessage.send_chat(self.payload.serialize())
        print 'Sent chat message'

    def render(self):
        return {
            'color' : 'blue'
        }

# msg received
class ChatMessageIn():

    def __init__(self, msg):
        self.payload = Payload(msg)
        self.payload.clean()
        self.valid = self.payload.valid()
        self.timestamp = int(now())

    # returns specific render formatting for the message (color, font etc)
    def render(self):
        return {
            'color' : 'blue'
        }

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
        self.time = int(kwargs.get('time', now()))
        self.channel = kwargs.get('channel', '')
        self.valid()

    def clean(self):
        pass

    # checks if all properties are in payload
    def valid(self, properties=None):
        if properties is None:
            properties = self.properties
        valid = True
        for p in properties:
            if getattr(self, p, None) is None:
                valid = False
        self.is_valid = valid
        return self.is_valid

    def serialize(self):
        d = {}
        for p in self.properties:
            d[p] = getattr(self, p, None)
        return d


# text entry manager
class ChatInput:

    MAX_MESSAGE_LENGTH = 140

    def __init__(self):
        self.clear()
        self.history = ChatInputHistory()
        self.processor = ChatInputProcessor()

    def clear(self):
        self.buffer = []
        self.cursor = 0
        
    def __str__(self):
        return ''.join(self.buffer)

    def add(self, char, index=None):
        if index is not None:
            self.buffer.insert(index, char)
        else:
            self.buffer.insert(self.cursor, char)
        self.cursor += 1

    def remove(self, index=None):
        try:
            if index is not None:
                self.buffer.pop(index)
            else:
                self.buffer.pop()
            self.cursor += -1
        except IndexError:
            pass

    def cursor_left(self):
        self.cursor = max(0, self.cursor - 1)

    def cursor_right(self):
        self.cursor = min(len(self.buffer), self.cursor + 1)

    def history_newer(self):
        self.buffer = list(self.history.newer())
        self.cursor = len(self.buffer)

    def history_older(self):
        self.buffer = list(self.history.older(self.buffer))
        self.cursor = len(self.buffer)

    def submit(self):
        text = str(self)
        self.clear()
        self.history.reset_index()
        self.history.add(text)
        print 'submitting ', text
        return text

    def process(self, key, symbol, modifiers):
        callback = self.processor.process(key, symbol, modifiers)
        if callable(callback):
            return callback(self)
        
# key input is routed to here
class ChatInputProcessor:

    def __init__(self):
        pass

    def process(self, key, symbol, modifiers):
        callback = None
        if symbol == key.ENTER:         # submit
            def callback(input):
                ChatClientGlobal.chatClient.send()
                return lambda keyboard: keyboard.toggle_chat()
        elif symbol == key.BACKSPACE:   # delete
            callback = lambda input: input.remove()
        elif symbol == key.UP:          # up history
            callback = lambda input: input.history_older()
        elif symbol == key.DOWN:        # down history
            callback = lambda input: input.history_newer()
        elif symbol == key.LEFT:        # move cursor
            callback = lambda input: input.cursor_left()
        elif symbol == key.RIGHT:       # move cursor
            callback = lambda input: input.cursor_right()
        else:                           # add character
            callback = self._add_char(key, symbol, modifiers) or callback
        return callback

    def _add_char(self, key, symbol, modifiers):
        try:
            c = chr(symbol)
        except ValueError:
            return None
        
        digit_punctuation_map = '!@#$%^&*()'
        lower_punctuation = '`-=[]\\;\',./'
        upper_punctuation = '~_+{}|:"<>?'
        
        if 'MOD_SHIFT' in key.modifiers_string(modifiers):
            if c in lowercase:
                c = c.upper()
            elif c in digits:
                c = digits_punctuation_map[digits.index(c)]
            elif c in lower_punctuation: # punctuation
                c = upper_punctuation[lower_punctuation.index(c)]

        return lambda input: input.add(c)
        
# history of submitted messages
class ChatInputHistory:

    MAX_HISTORY = 50

    def __init__(self):
        self.clear()
        self.active_buffer = ''

    def clear(self):
        self.buffer = deque([], self.MAX_HISTORY)
        self.reset_index()

    def reset_index(self):
        self.current_index = -1

    def newer(self):
        self.current_index = max(-1, self.current_index - 1)
        return self.selected()

    def older(self, active_buffer=''):
        if self.current_index == -1:    # save active typed-in message that has not been sent yet
            self.active_buffer = str(active_buffer)
        self.current_index = min(len(self.buffer) - 1, self.current_index + 1)
        return self.selected()

    def selected(self):
        if self.current_index == -1:
            return self.active_buffer
        else:
            return self.buffer[self.current_index]

    def add(self, text):
        self.buffer.appendleft(text)
        
# returns data for rendering
class ChatRender:

    MESSAGE_RENDER_TIMEOUT = 2500 # 2.5 seconds display time
    MESSAGE_RENDER_COUNT_MAX = 10 # max msgs to display at once

    def __init__(self):
        pass

    # return subscribed channel names
    def channel_names(self):
        return ChatClientGlobal.chatClient.subscriptions.keys()

    # returns messages to be rendered for the current active channel
    def messages(self, channel=None):
        client = ChatClientGlobal.chatClient
        if channel is None:
            channel = client.CURRENT_CHANNEL
        msgs = client.subscriptions[channel]
        to_render = deque([], self.MESSAGE_RENDER_COUNT_MAX)
        i = 0
        for msg in msgs:
            print msg
            if msg.timestamp - int(now()) > self.MESSAGE_RENDER_TIMEOUT or \
               i == self.MESSAGE_RENDER_COUNT_MAX:
                break
            to_render.appendleft(msg)
            i += 1
        return to_render

    def user_input(self):
        return str(ChatClientGlobal.chatClient.input)
        
if __name__ == '__main__':
    ChatClientGlobal.init_0()
    ChatClientGlobal.init_1()
