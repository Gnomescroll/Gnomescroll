#!/usr/bin/python

'''
Chat client
'''

from time import time
from collections import deque
from pyglet.window import key

def now():
    return int(time() * 1000)

class ChatClientGlobal:
    chatClient = None

    @classmethod
    def init_0(self): #first pass is declaring
        ChatClientGlobal.chatClient = ChatClient()
        ChatClientGlobal.chatRender = ChatRender()

    @classmethod
    def init_1(self): #calls import methods if needed
        pass

    @classmethod
    def on_identify(self): # called after client connects
        ChatClientGlobal.chatClient.on_identify()


from net_client import NetClientGlobal
from net_out import NetOut
from net_event import NetEventGlobal
from game_state import GameStateGlobal


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

    def __init__(self):
        self.input = ChatInput()
        print '__init__ subscribe'
        self.subscribe('system')
        print 'done'

    def on_identify(self, channel=None):
        print 'post identify subscribe'
        self._initial_subscribe(self.CURRENT_CHANNEL)
        print 'done'

    def _initial_subscribe(self, channel=None, _subscribed=[False]):
        loaded_channels = []
        if not _subscribed[0]:
            loaded_channels = self.load()
            if channel is not None and channel not in loaded_channels:
                self.set_current_channel(channel)
            elif self.CURRENT_CHANNEL not in loaded_channels:
                self.subscribe(self.CURRENT_CHANNEL)
            loaded_channels.append(self.CURRENT_CHANNEL)
            _subscribed[0] = True
            if 'system' not in loaded_channels:
                self.subscribe('system')
        return loaded_channels

    def set_current_channel(self, channel):
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
        NetOut.chatMessage.subscribe(channel)
        print 'Chat client subscribed to %s' % (channel,)

    def unsubscribe(self, channel=None):
        switch = False
        if channel == self.CURRENT_CHANNEL:
            switch = True
            if len(self.subscriptions) <= 1:
                return
        elif channel is None:
            channel = self.CURRENT_CHANNEL
            switch = True
            if len(self.subscriptions) <= 1:
                return
        if channel == 'system':
            return
        if channel in self.subscriptions:
            del self.subscriptions[channel]
            NetOut.chatMessage.unsubscribe(channel)
        if switch:
            channel_names = self.subscriptions.keys()
            next = channel_names[-1]
            if next == 'system' and len(channel_names) > 1:
                next = channel_names[-2]
            self.set_current_channel(next)

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
        if text == '':
            return
        if text[0] == '/':
            msg = ChatCommand(text)
        else:
            msg = ChatMessageOut(text)
        msg.send()

    # receive incoming message
    def receive(self, msg):
        print 'Received msg:'
        print msg
        channel = msg.get('channel', None)
        if channel is None:
            return
        if channel in self.subscriptions:
            msg = ChatMessageIn(msg)
            if msg.payload.cid in self.ignored:
                return
            self.subscriptions[channel].receive(msg)

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
        print 'loading channels from file'
        conf = CONFIG_PATH + '/' + self._conf
        channels = []
        if path_exists(conf):
            with open(conf, 'r') as f:
                settings = decode_json(f.read())
                print settings
                ignored = settings.get('ignored', None)
                if ignored is not None:
                    self.ignored = ignored
                channels = settings.get('subscriptions', channels)
                print channels
                for channel in channels:
                    print channel
                    self.subscribe(channel)
        print 'done'
        return channels

    def system_notify(self, txt):
        SystemChatCommand(txt).send()

    def insert_string(self, txt):
        self.input.insert_string(txt)


# channel wrapper
class Channel:

    HISTORY_MAX = 200

    def __init__(self, name):
        self.name = name
        self.history = deque([], self.HISTORY_MAX)
        self._curr_iter = 0

    def next(self):
        if self._curr_iter == len(self.history):
            self._curr_iter = 0
            raise StopIteration
        else:
            self._curr_iter += 1
            return self.history[self._curr_iter-1]

    def __iter__(self):
        return self

    def reset_iter(self):
        self._curr_iter = 0

    def receive(self, msg):
        if msg.valid:
            self.history.appendleft(msg)

# special channel for system msgs (has extra processing specific to /commands)
class SystemChannel(Channel):

    def receive(self, msg):
        if msg.payload.content == 'ping':
            content = 'Chat ping round-trip time = %ims' % (now()-int(msg.payload.time),)
            log = ChatMessageIn({
                'content'   : content,
                'channel'   : 'system',
                'cid'       : 'system',
                'cmd'       : 'chat',
            })
        else:
            log = msg
            
        if log.valid:
            self.history.appendleft(log)

# command message (e.g. /channel highlands i am messaging the highlands channel)
class ChatCommand():

    def __init__(self, text):
        text_pts = text.split(' ')
        command = text_pts[0][1:]
        args = text_pts[1:]
        self._send = None
        self.payload = None
        self.route(command, args)

    # create a special payload and/or a special _send command
    def route(self, command, args):
        _send = None
        payload = None

        if command == 'channel':
            payload = Payload(
                channel = args[0],
                content = str(' '.join(args[1:])),
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
            if len(args) < 1:
                return
            _send = lambda: ChatClientGlobal.chatClient.set_current_channel(args[0])
        elif command == 'leave':
            if len(args) == 0:
                channel = None
            else:
                channel = args[0]
            _send = lambda: ChatClientGlobal.chatClient.unsubscribe(channel)
        elif command == 'nick':
            if len(args) < 1:
                return
            _send = lambda: NetOut.sendMessage.identify(args[0])
        elif command == 'pm':
            if len(args) < 1:
                return
            client_id = GameStateGlobal.playerList.by_name(args[0])
            if not client_id:
                _send = self._send_local({
                    'content'   :   'Cannot msg unknown player: %s' % (args[0],),
                    'channel'   :   'system',
                })
            else:
                payload = Payload(
                    channel = 'pm_' + client_id,
                    content = ' '.join(args[1:]),
                )
                
        else:
            _send = self._unimplemented(command)

        self.payload = payload
        self._send = _send

    def _unimplemented(self, command):
        return self._send_local({
            'content' : command + ' command is not implemented.',
            'channel' : 'system'
        })

    def _send_local(self, data):
        def _send():
            print 'sending local'
            ChatClientGlobal.chatClient.receive(Payload(**data).serialize())
        return _send

    def send(self):
        if self._send is not None:
            self._send()
        else:
            if self.payload is not None:
                NetOut.chatMessage.send_chat(self.payload.serialize())

# automatic messages sent by the program without user prompt
class SystemChatCommand(ChatCommand):

    def route(self, command, args):
        _send = None
        payload = None

        if command == 'identify_fail':
            _send = self._send_local(' '.join(args))
        else:
            _send = self._unimplemented(command)

        self.payload = payload
        self._send = _send

    def _send_local(self, data):
        if type(data) == str:
            data = {
                'content' : data,
            }
        if 'channel' not in data:
            data['channel'] = 'system'
        return ChatCommand._send_local(self, data)

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

    def __init__(self, msg=None):
        if msg is None:
            msg = {}
        self.payload = Payload(**msg)
        self.payload.clean()
        self.valid = self.payload.valid()
        self.timestamp = now()
        print 'chatmessageIN timestamp %i' % (self.timestamp,)

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
        'cid',
        'id',
    ]
    
    def __init__(self, **msg):
        #required
        self.cmd = msg.get('cmd', 'chat')
        self.content = msg.get('content', '')
        self.time = int(msg.get('time', now()))
        self.channel = msg.get('channel', '')
        self.cid = msg.get('cid', NetClientGlobal.client_id)
        print msg.get('cid')
        print msg
        print NetClientGlobal.client_id
        self.valid()
        #optional
        self.id = msg.get('id', '')
    def clean(self):
        pass

    # checks if all properties are in payload
    def valid(self, properties=None):
        self.is_valid = True
        return self.is_valid
        #if properties is None:
            #properties = self.properties
        #valid = True
        #for p in properties:
            #if hasattr(self, p):
                #valid = False
        #self.is_valid = valid
        #return self.is_valid

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
        self.text = ''

    def __str__(self):
        #return ''.join(self.buffer)
        return self.text

    def stringify(self):
        self.text = ''.join(self.buffer)

    def insert_string(self, txt):
        for c in txt:
            self.add(c)

    def add(self, char, index=None):
        if index is not None:
            self.buffer.insert(index, char)
        else:
            self.buffer.insert(self.cursor, char)
        self.cursor += 1
        self.stringify()

    def remove(self, index=None):
        try:
            if index is not None:
                self.buffer.pop(index)
            else:
                self.buffer.pop()
            self.cursor += -1
            self.stringify()
        except IndexError:
            pass

    def cursor_left(self):
        self.cursor = max(0, self.cursor - 1)

    def cursor_right(self):
        self.cursor = min(len(self.buffer), self.cursor + 1)

    def history_newer(self):
        self.buffer = list(self.history.newer())
        self.stringify()
        self.cursor = len(self.buffer)

    def history_older(self):
        self.buffer = list(self.history.older(str(self)))
        self.stringify()
        self.cursor = len(self.buffer)

    def submit(self):
        text = str(self)
        self.clear()
        self.history.reset_index()
        self.history.add(text)
        print 'submitting ', text
        return text

    def _input_callback(self, callback):
        if callable(callback):
            return callback(self)
            
    def on_key_press(self, symbol, modifiers):
        callback = self.processor.on_key_press(symbol, modifiers)
        return self._input_callback(callback)

    def on_text(self, text):
        callback = self.processor.on_text(text)
        return self._input_callback(callback)
        
    def on_text_motion(self, motion):
        callback = self.processor.on_text_motion(motion)
        return self._input_callback(callback)

# key input is routed to here
class ChatInputProcessor:

    def __init__(self):
        pass

    def on_key_press(self, symbol, modifiers):
        callback = None
        if symbol == key.ENTER:         # submit
            def callback(input):
                ChatClientGlobal.chatClient.send()
                return lambda keyboard: keyboard.toggle_chat()
        elif symbol == key.ESCAPE:      # clear, cancel chat
            def callback(input):
                input.clear()
                return lambda keyboard: keyboard.toggle_chat()
        return callback

    def on_text(self, text):
        return lambda input: input.add(text)

    def on_text_motion(self, motion):
        motion = key.motion_string(motion)
        callback = None
        if motion == 'MOTION_UP':            # up history
            callback = lambda input: input.history_older()
        elif motion == 'MOTION_DOWN':        # down history
            callback = lambda input: input.history_newer()
        elif motion == 'MOTION_LEFT':        # move cursor
            callback = lambda input: input.cursor_left()
        elif motion == 'MOTION_RIGHT':       # move cursor
            callback = lambda input: input.cursor_right()
        elif motion == 'MOTION_BACKSPACE':   # delete
            callback = lambda input: input.remove()
        return callback

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

    MESSAGE_RENDER_TIMEOUT = 3500 # 2.5 seconds display time
    MESSAGE_RENDER_COUNT_MAX = 8 # max msgs to display at once

    def __init__(self):
        self.empty_message = ChatMessageIn({ 'content': ''})

    # return subscribed channel names
    def channel_names(self):
        return ChatClientGlobal.chatClient.subscriptions.keys()

    # returns messages to be rendered for the current active channel
    def messages(self, channel=None):
        if channel is None:
            channel = ChatClientGlobal.chatClient.CURRENT_CHANNEL
        to_render = self._filter_channel(channel)
        if channel != 'system':
            to_merge = self._filter_channel('system')
            to_render = self._merge_channels(to_render, to_merge)
        self._pad_queue(to_render)
        return to_render

    # filter channel to recent messages, maximum self.MESSAGE_RENDER_COUNT_MAX
    def _filter_channel(self, channel):
        client = ChatClientGlobal.chatClient
        to_render = deque([], self.MESSAGE_RENDER_COUNT_MAX)
        msgs = client.subscriptions.get(channel, None)
        if msgs is None:
            print 'Attempted to retrieve messages for channel: %s, but does not exist' % (channel,)
            return to_render
        to_render = deque([], self.MESSAGE_RENDER_COUNT_MAX)
        i = 0
        for msg in msgs:
            if now() - msg.timestamp > self.MESSAGE_RENDER_TIMEOUT or \
               i == self.MESSAGE_RENDER_COUNT_MAX:
                break
            to_render.appendleft(msg)
            i += 1
        msgs.reset_iter()
        return to_render

    # merge two channel queues, sorted by time
    def _merge_channels(self, deque1, deque2):
        merged = list(deque1)
        merged.extend(list(deque2))
        merged = sorted(merged, key=lambda msg: msg.timestamp)
        merged = merged[0:self.MESSAGE_RENDER_COUNT_MAX]
        merged = deque(merged, self.MESSAGE_RENDER_COUNT_MAX)
        return merged

    # pads queue with empty messages
    def _pad_queue(self, queue):
        queue.extend([self.empty_message for j in range(self.MESSAGE_RENDER_COUNT_MAX - len(queue))])
        
    def user_input(self):
        return str(ChatClientGlobal.chatClient.input)

    def cursor_position(self):
        return ChatClientGlobal.chatClient.input.cursor

if __name__ == '__main__':
    ChatClientGlobal.init_0()
    ChatClientGlobal.init_1()
