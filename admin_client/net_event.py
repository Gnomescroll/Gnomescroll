#!/usr/bin/python

'''
Admin Client network incoming
'''

import json
import SDL.gl

class NetEventGlobal:
    messageHandler = None
    clientMessageHandler = None
    miscMessageHandler = None
    
    @classmethod
    def init(cls):
        cls.messageHandler = MessageHandler()
        cls.clientMessageHandler = ClientMessageHandler()
        cls.miscMessageHandler = MiscMessageHandler()
        
    @classmethod
    def register_json_events(cls, events):
        for string, function in events.items():
            cls.messageHandler.json_events[string] = function

class MessageHandler:
    player = None #move this somewhere else
    @classmethod
    def init(self):
        pass
    def __init__(self):
        self.json_events = {} #map strings to functions

    def process_net_event(self, msg_type, datagram):
        if msg_type == 1:       #json message
            self.process_json_event(msg_type, datagram)
        else:                   #create a process json message
            self.process_binary_event(msg_type, datagram)

#binary events
    def process_binary_event(self, msg_type, datagram):
        if msg_type == 3:
            NetEventGlobal.mapMessageHandler._map_chunk(datagram)
        elif msg_type == 4:
            self._4_
        else:
            print "MessageHandler.process_binary_event: message type unknown, " + str(msg_type)
#message events

    def process_json_event(self, msg_type, datagram):
        try:
            msg = json.loads(datagram)
        except Exception, e:
            print "MessageHandler.process_json_event error"
            print e
            return
        cmd = msg.get('cmd', None)
        if cmd is None:
            return
        #use json_events when possible
        cmd = str(cmd)
        if cmd in self.json_events:
            self.json_events[cmd](**msg)
        else:
            print "Error, received command %s that client cannot handle" % (cmd,)

class GenericMessageHandler:

    events = {}

    def register_events(self):
        NetEventGlobal.register_json_events(self.events)

    def _assign_events_to_methods(self):
        for event, name in self.events.items():
            ev = getattr(self, str(name), None)
            if ev is not None:
                self.events[event] = ev

    def __init__(self):
        self._assign_events_to_methods()
        self.register_events()


class MiscMessageHandler(GenericMessageHandler):
    events = {
        'ping' : '_ping',
    }

    def _ping(self, **msg):
        print '%d ms' % (SDL.gl.get_ticks() - msg['timestamp'],)

class ClientMessageHandler(GenericMessageHandler):

    events = {
        'client_id' : '_client_id',
        'set_client_id' : '_set_client_id',
        'client_quit' : '_client_quit',
    }

    def _client_id(self, **msg):
        if not self._set_client_id(**msg):
            NetOut.sendMessage.request_client_id()

    def _set_client_id(self, id, **arg):
        print "Received Client Id: %s" % (id,)
        NetClientGlobal.client_id = id
        return True

    def _client_quit(self, id, **msg):
        GameStateGlobal.client_quit(id)
        

from net_client import NetClientGlobal
from net_out import NetOut
