#!/usr/bin/python

'''
Client network incoming
'''

import simplejson as json
#import struct

class NetEventGlobal:
    messageHandler = None
    clientMessageHandler = None
    mapMessageHandler = None
    projectileMessageHandler = None
    agentMessageHandler = None
    playerMessageHandler = None
    @classmethod
    def init_0(cls):
        cls.messageHandler = MessageHandler()
        cls.clientMessageHandler = ClientMessageHandler()
        cls.playerMessageHandler = PlayerMessageHandler()
        cls.chatMessageHandler = ChatMessageHandler()

        cls.agentMessageHandler = AgentMessageHandler()
        cls.mapMessageHandler = MapMessageHandler()
        cls.projectileMessageHandler = ProjectileMessageHandler()

    @classmethod
    def init_1(cls):
        pass
        MessageHandler.init()
        ClientMessageHandler.init()
        PlayerMessageHandler.init()
        ChatMessageHandler.init()

        AgentMessageHandler.init()
        MapMessageHandler.init()
        ProjectileMessageHandler.init()

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
        except:
            print "MessageHandler.process_json_event error"
            print datagram
            return
        cmd = msg.get('cmd', None)
        if cmd is None:
            return
        #use json_events when possible
        if cmd in self.json_events:
            self.json_events[cmd](**msg)

class GenericMessageHandler:

    events = {}

    def register_events(self):
        NetEventGlobal.register_json_events(self.events)

    def _assign_events_to_methods(self):
        for event, name in self.events.items():
            if type(name) == str:
                self.events[event] = getattr(self, name)
            
    @classmethod
    def init(cls):
        pass
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


class MapMessageHandler(GenericMessageHandler):
    terrainMap = None
    mapChunkManager = None
    mapController = None

    events = {
        'chunk_list' : '_chunk_list',
        'map_chunk' : '_map_chunk',
        'set_map' : '_set_map',
    }

    @classmethod
    def init(cls):
        cls.terrainMap = GameStateGlobal.terrainMap
        cls.mapChunkManager = MapChunkManagerGlobal.mapChunkManager
        cls.mapController = MapControllerGlobal.mapController
        assert cls.mapController != None

    def _chunk_list(self, list, **msg):
        #print str(list)
        self.mapController.process_chunk_list(list)
        #for chunk in list:
        #    (x,y,z,version ) = chunk

    def _map_chunk(self, datagram):
        #print "Map Chunk Received"
        (x,y,z) = self.terrainMap.set_packed_chunk(datagram)
        self.mapChunkManager.set_map(x,y,z) #tells to redraw chunk
        self.mapController.incoming_map_chunk(x,y,z)

    def _set_map(self, list, **msg):
        for x,y,z,value in list:
            self.terrainMap.set(x,y,z,value)
            self.mapChunkManager.set_map(x,y,z) #redraw chunk


class ClientMessageHandler(GenericMessageHandler):

    events = {
        'client_id' : '_client_id',
        'set_client_id' : '_set_client_id',
        'client_quit' : '_client_quit',
        'identified' : '_identified',
        'identify_fail' : '_identify_fail',            
    }

    def _client_id(self, **msg):
        if self._set_client_id(**msg):
            NetOut.sendMessage.identify()
        else:
            NetOut.sendMessage.request_client_id()

    def _set_client_id(self, id, **arg):
        print "Received Client Id: %s" % (id,)
        NetClientGlobal.client_id = id
        return True

    def _client_quit(self, id, **msg):
        GameStateGlobal.client_quit(id)

    def _identified(self, **msg):
        note = msg.get('msg', '')
        ChatClientGlobal.chatClient.system_notify('/identify_note ' + note)
        player = msg.get('player', None)
        if player is None:
            print 'msg::identified - missing player'
            return False
        name = player.get('name', None)
        if name is None:
            print 'msg::identified - player missing name'
            return False
        if type(name) != str:
            print 'msg::identified - name is not str'
            return False
        client_id = player.get('cid', None) # client_id is currently optional for server to send
        if client_id is not None:
            NetClientGlobal.client_id = client_id
        NetClientGlobal.name = name
        print 'Identified: name is %s' % (name,)
        ChatClientGlobal.on_identify()
        player = GameStateGlobal.update_your_info(player)
        if player.you:
            GameStateGlobal.playerList.identify(player)
        return True

    def _identify_fail(self, msg, **arg):
        # send system notification
        ChatClientGlobal.chatClient.system_notify('/identify_fail '+msg)
        ChatClientGlobal.chatClient.system_notify('/identify_fail Use /nick to set name.')
        # activate chat, insert /nick
        InputGlobal.enable_chat()
        ChatClientGlobal.chatClient.insert_string('/nick ')


# base class for datastore (*Lists) network interface
class DatastoreMessageInterface(GenericMessageHandler):

    event_extensions = ['list', 'update', 'destroy', 'create']
    name = ''
    store = None

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
        if data is None:
            err_msg = '%s key missing' % (self.name,)
        if err_msg is not None:
            print self._error_message(err_msg, **args)
            return
        self.store.load_info(**data)

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
        self.store.load_list(a_list)

    def _default_create(self, **args):
        err_msg = None
        data = args.get(self.name, None)
        if data is None:
            err_msg =  '%s key missing' % (self.name,)
        if err_msg is not None:
            print self._error_message(err_msg, **args)
            return
        self.store.create(**data)

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


class PlayerMessageHandler(DatastoreMessageInterface):

    def __init__(self):
        self.name = 'player'
        self.store = GameStateGlobal.playerList
        DatastoreMessageInterface.__init__(self)

    def _player_destroy(self, **args):
        err_msg = None
        id = args.get('id', None)
        if id is None:
            err_msg = 'id is missing'
        if err_msg is not None:
            print self._error_message(err_msg, **args)
            return
        GameStateGlobal.remove_player(id)    # this method manages FK relationships

# agent messages needs to be updated
# there is no agent_create, and agent_destroy is called remove_agent
class AgentMessageHandler(DatastoreMessageInterface):

    def __init__(self):
        self.name = 'agent'
        self.store = GameStateGlobal.agentList
        self._bind_event('agent_position', self._agent_position)
        DatastoreMessageInterface.__init__(self)

    def _agent_position(self, **args):  # deprecate
        state = args.get('state', None)
        id = args.get('id', None)
        tick = args.get('tick', None)
        if None in (state, id, tick,):
            print 'agent_position, missing keys'
            print args
            return

        agent = GameStateGlobal.agentList[id]
        if agent is None: # agent not found, request agent
            NetOut.sendMessage.request_agent(id)
            return
        agent.tick = tick
        agent.state = state

    def _agent_destroy(self, **args):
        err_msg = None
        id = args.get('id', None)
        if id is None:
            err_msg = 'id is missing'
        if err_msg is not None:
            print self._error_message(err_msg, **args)
            return
        GameStateGlobal.remove_agent(id)    # this method manages FK relationships
        
    
class WeaponMessageHandler(DatastoreMessageInterface):

    def __init__(self):
        self.name = 'weapon'
        self.store = GameStateGlobal.weaponList
        DatastoreMessageInterface.__init__(self)


class ProjectileMessageHandler(DatastoreMessageInterface):

    def __init__(self):
        self.name = 'projectile'
        self.store = GameStateGlobal.projectileList
        DatastoreMessageInterface.__init__(self)

from game_state import GameStateGlobal
from net_client import NetClientGlobal
from net_out import NetOut
from chat_client import ChatClientGlobal
from map_chunk_manager import MapChunkManagerGlobal
from map_controller import MapControllerGlobal
from input import InputGlobal
