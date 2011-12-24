#!/usr/bin/python

'''
Client network incoming
'''
import opts
opts = opts.opts

import json
import zlib

import c_lib.c_lib_sdl
import stats
import animations

from dat_loader import dat_loader

class NetEventGlobal:
    messageHandler = None
    clientMessageHandler = None
    mapMessageHandler = None
    agentMessageHandler = None
    miscMessageHandler = None
    weaponMessageHandler = None
    datMessageHandler = None

    @classmethod
    def init(cls):
        cls.messageHandler = MessageHandler()
        cls.clientMessageHandler = ClientMessageHandler()
        cls.chatMessageHandler = ChatMessageHandler()
        cls.miscMessageHandler = MiscMessageHandler()

        cls.agentMessageHandler = AgentMessageHandler()
        cls.mapMessageHandler = MapMessageHandler()
        cls.weaponMessageHandler = WeaponMessageHandler()

        cls.datMessageHandler = DatMessageHandler()

    @classmethod
    def register_json_events(cls, events):
        for string, function in events.items():
            cls.messageHandler.json_events[string] = function

class MessageHandler:
    def __init__(self):
        self.json_events = {} #map strings to functions

    def process_net_event(self, msg_type, datagram):
        if msg_type == 1:       #json message
            self.process_json_event(msg_type, datagram)
        elif msg_type == 3:                   #create a process json message
            self.process_binary_event(msg_type, datagram)
        elif msg_type == 4:
            self.process_json_compressed_event(msg_type, datagram)
        else:
            print "WARNING: MessageHandler:: unknown msg_type %d" % (msg_type,)

#binary events
    def process_binary_event(self, msg_type, datagram):
        if msg_type == 3:
            NetEventGlobal.mapMessageHandler._map_chunk(datagram)
        elif msg_type == 4:
            raise Exception, "msg_type 4 not implemented"
        else:
            print "MessageHandler.process_binary_event: message type unknown, " + str(msg_type)

#message events
    def process_json_event(self, msg_type, datagram):
        try:
            msg = json.loads(datagram)
        except Exception, e:
            print "MessageHandler.process_json_event error"
            print e
            #import sys
            #sys.exit()
            print datagram
            return
        cmd = msg.get('cmd', None)
        if cmd is None:
            return
        #use json_events when possible
        cmd = str(cmd)
        if cmd in self.json_events:
            #print msg
            self.json_events[cmd](**msg)
        else:
            print "Error, received command %s that client cannot handle" % (cmd,)
            print 'msg %s' % (msg,)
            print self.json_events.keys()
            assert False

    def process_json_compressed_event(self, msg_type, datagram):
        try:
            msg = zlib.decompress(datagram)
        except Exception, e:
            print "MessageHandler zlib decompression failed"
            print e
            return
        self.process_json_event(msg_type, msg)

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


class ChatMessageHandler(GenericMessageHandler):

    events = {
        'chat' : '_chat',
        'you_died' : '_you_died',
        'you_killed' : '_you_killed',
    }

    def _chat(self, **msg):
        ChatClientGlobal.chatClient.receive(msg)

    def _you_died(self, **msg):
        ChatClientGlobal.chatClient.system_notify(msg['msg'])

    def _you_killed(self, **msg):
        ChatClientGlobal.chatClient.system_notify(msg['msg'])


class MiscMessageHandler(GenericMessageHandler):
    events = {
        'ping' : '_ping',
    }

    def _ping(self, timestamp, **msg):
        stats.last_ping = c_lib.c_lib_sdl.get_ticks() - timestamp

class MapMessageHandler(GenericMessageHandler):

    events = {
        'chunk_list' : '_chunk_list',
        'map_chunk' : '_map_chunk',
        'set_map' : '_set_map',
        'clear_map': '_clear_map',
    }

    def _chunk_list(self, list, **msg):
        print "chunk list"
        MapControllerGlobal.mapController.process_chunk_list(list)

    def _map_chunk(self, datagram):
        #print "Map Chunk Received"
        (x,y,z) = terrainMap.set_packed_chunk(datagram)
        MapControllerGlobal.mapController.incoming_map_chunk(x,y,z)

    def _set_map(self, list, **msg):
        for x,y,z,value in list:
            terrainMap.set(x,y,z,value)
            if value == 0:
                animations.BlockCrumbleAnimation([x,y,z]).play()

    def _clear_map(self, **msg):
        terrainMap.clear()


class ClientMessageHandler(GenericMessageHandler):

    events = {
        'client_quit' : '_client_quit',
        'identified' : '_identified',
        'identify_fail' : '_identify_fail',
    }

    def _client_quit(self, id, **msg):
        #GameStateGlobal.client_quit(id)
        pass

    def _identified(self, **msg):
        note = msg.get('msg', '')
        ChatClientGlobal.chatClient.system_notify('/identify_note ' + note)

        name = msg.get('name', None)
        if name is None:
            print 'msg::identified - player missing name'
            return False
        name = str(name)
        if not name:
            print 'msg identified :: name is empty'
            return False

        NetClientGlobal.name = name
        print 'Identified: name is %s' % (name,)
        ChatClientGlobal.on_identify()

        return True

    used_alt = False
    def _identify_fail(self, msg, **arg):
        # send system notification
        if self.used_alt:
            ChatClientGlobal.chatClient.system_notify('/identify_fail '+msg)
            ChatClientGlobal.chatClient.system_notify('/identify_fail Use /nick to set name.')
            # activate chat, insert /nick
            InputGlobal.toggle_chat()
            ChatClientGlobal.chatClient.insert_string('/nick ')
        else:
            NetOut.sendMessage.identify(name=opts.alt_name)
            self.used_alt = True

# base class for datastore (*Lists) network interface
class DatastoreMessageInterface(GenericMessageHandler):

    event_extensions = ['list', 'update', 'destroy', 'create']
    name = ''
    store = None

    # provide the NetOut request method for relevant object type
    # will call this on object update failure (attempt to update unknown object)
    def request_data(self, **data):
        pass

    def __init__(self):
        self._load_default_events()
        GenericMessageHandler.__init__(self)

    def _load_default_events(self):
        for evex in self.event_extensions:
            event_name = '%s_%s' % (self.name, evex,)
            method_name = '_' + event_name
            method = getattr(self, method_name, None)
            if method is None:
                method_name = '_default_%s' % (evex,)
                method = getattr(self, method_name)
            self._bind_event(event_name, method)

    def _bind_event(self, event_name, method):
        self.events[event_name] = method

    def _error_message(self, info_string, **msg):
        return 'msg %s :: %s' % (msg['cmd'], info_string,)

    def _default_update(self, **args):
        err_msg = None
        data = args.get(self.name, None)
        full = args.get('full', 0)
        if data is None:
            err_msg = '%s key missing' % (self.name,)
        if err_msg is not None:
            print self._error_message(err_msg, **args)
            return

        if full:
            obj = self.store.update_or_create(**data)
        else:
            obj = self.store.load_info(**data)
        #if obj is False:
            #self.request_data(**data)
        return obj

    def _default_list(self, **args):
        err_msg = None
        list_key = '%s_list' % (self.name,)
        try:
            a_list = args[list_key]
            assert type(a_list) == list
        except KeyError:
            err_msg = '%s key missing' % (list_key,)
        except AssertionError:
            err_msg = '%s is not a list' % (list_key,)
        if err_msg is not None:
            print self._error_message(err_msg, **args)
            return
        objs = self.store.load_list(a_list)
        return objs

    def _default_create(self, **args):
        err_msg = None
        data = args.get(self.name, None)
        if data is None:
            err_msg =  '%s key missing' % (self.name,)
        if err_msg is not None:
            print self._error_message(err_msg, **args)
            return
        obj = self.store.create(**data)
        return obj

    def _default_destroy(self, **args):
        err_msg = None
        try:
            id = int(args.get('id', None))
            self.store.destroy(id)
        except TypeError:
            err_msg = '%s id missing' % (self.name,)
        except ValueError:
            err_msg = '%s id invalid' % (self.name,)
        except KeyError:
            err_msg = '%s not found' % (self.name,)
        if err_msg is not None:
            print self._error_message(err_msg, **args)
            return
        return id

# agent messages needs to be updated
# there is no agent_create, and agent_destroy is called remove_agent
class AgentMessageHandler(DatastoreMessageInterface):

    def __init__(self):
        self.name = 'agent'
        self.store = GameStateGlobal.agentList
        DatastoreMessageInterface.__init__(self)

    def request_data(self, **data):
        if 'id' in data:
            NetOut.sendMessage.request_agent(data['id'])

    def _agent_destroy(self, **args):
        id = self._default_destroy(**args)
        if id is not None:
            GameStateGlobal.remove_agent(id)    # this method manages FK relationships

    def _agent_list(self, **args):
        self._default_list(**args)


class WeaponMessageHandler(DatastoreMessageInterface):

    def __init__(self):
        self.name = 'weapon'
        self.store = GameStateGlobal.weaponList
        DatastoreMessageInterface.__init__(self)

    def request_data(self, **data):
        if 'id' in data:
            NetOut.sendMessage.request_weapon(data['id'])

    def _weapon_destroy(self, **args):
        id = self._default_destroy(**args)
        if id is not None:
            GameStateGlobal.remove_weapon(id)

    def _weapon_update(self, **args):
        return self._default_update(**args)


class DatMessageHandler(GenericMessageHandler):
    events = {
        'dat'   :   '_load_dat',
    }

    def _load_dat(self, **msg):
        err_msg = None
        try:
            dat = msg['dat']
        except KeyError:
            err_msg = 'dat missing'

        if err_msg is not None:
            print 'msg dat :: %s' % (err_msg,)
            return

        name, type, key = None, None, None
        if 'name' in msg:
            name = msg['name']
        if 'type' in msg:
            name = msg['type']
        if 'key' in msg:
            key = msg['key']

        if name is not None:
            if type is not None and key is not None:
                self._load_type_key(self, name, type, key, dat)
            else:
                self._load_name(self, name, dat)
        else:
            self._load_all(dat)

        if dat_loader.fully_loaded:
            print "Dats are loaded"
            NetOut.datMessage.loaded()

    def _load_all(self, dat):
        dat_loader.load_all(dat)
        NetOut.datMessage.loaded()

    def _load_name(self, name, dat):
        dat_loader.load(name, dat)

    def _load_type_key(self, name, type, key, val):
        dat_loader.set_property(name, type, key, val)

from game_state import GameStateGlobal
from net_client import NetClientGlobal
from net_out import NetOut
from chat_client import ChatClientGlobal
from map_controller import MapControllerGlobal
from input import InputGlobal

import c_lib.terrain_map as terrainMap

import animations
