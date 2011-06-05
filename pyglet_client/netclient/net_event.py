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
        self.player = GameStateGlobal.player
        self.agent = GameStateGlobal.agent
        assert self.player != None
        assert self.agent != None
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
        if self.json_events.has_key(cmd):
            self.json_events[cmd](**msg)

class GenericMessageHandler:

    events = {}

    def register_events(self):
        NetEventGlobal.register_json_events(self.events)
    @classmethod
    def init(cls):
        pass
    def __init__(self):
        self.register_events()

class ChatMessageHandler(GenericMessageHandler):

    events = {
        'chat' : self._chat,
        'you_died' : self._you_died,
        'you_killed' : self._you_killed,
    }

    def _chat(self, **msg):
        ChatClientGlobal.chatClient.receive(msg)

    def _you_died(self, **msg):
        ChatClientGlobal.chatClient.system_notify(msg['msg'])

    def _you_killed(self, **msg):
        ChatClientGlobal.chatClient.system_notify(msg['msg'])

class ClientMessageHandler(GenericMessageHandler):

    events = {
        'client_id' : self._client_id,
        'set_client_id' : self._set_client_id,
        'client_quit' : self._client_quit,
        'identified' : self._identified,
        'identify_fail' : self._identify_fail,            
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

class PlayerMessageHandler(GenericMessageHandler):

    events = {
        'player_list' : self._player_list,
        'player_info' : self._player_info,
        'remove_player' : self._remove_player,
        'player_update' : self._player_update,
        'update_player' : self._update_player,
    }

    def _player_list(self, players, **args):
        try:
            assert type(players) == list
        except AssertionError:
            print 'msg player_list :: players is not a list'
            return
        GameStateGlobal.playerList.load_list(players)

    def _player_info(self, **arg):
        if not self._update_player(**arg):
            return

    def _player_update(self,**arg):
        if not self._update_player(**arg):
            return

    def _remove_player(self, id, **arg):
        GameStateGlobal.remove_player(id)

    def _update_player(self, player, **msg):
        try:
            assert type(player) == dict
        except AssertionError:
            print 'msg player_info :: player is not a dict'
            return False
        GameStateGlobal.playerList.load_info(**player)
        return True


class AgentMessageHandler(GenericMessageHandler):

    events = {
        'agent_position' : self._agent_position,
        'agent_list'    :   self._agent_list,
        'agent_info'    :   self._agent_info,
        'agent_update' : self._agent_update,
        'remove_agent' : self._remove_agent,
    }

    def _agent_position(self, **args):
        state = args.get('state', None)
        id = args.get('id', None)
        tick = args.get('tick', None)
        if None in (state, id, tick,):
            print 'agent_position, missing keys'
            print args
            return
        x,y,z, vx,vy,vz, ax,ay,az = state
        x,y,z = map(lambda k: float(k), [x,y,z])

        agent = GameStateGlobal.agentList[id]
        if agent is None: # agent not found, request agent
            NetOut.sendMessage.request_agent(id)
            return
        agent.tick = tick
        agent.x = x
        agent.y = y
        agent.z = z
        agent.vx = vx
        agent.vy = vy
        agent.vz = vz
        agent.ax = ax
        agent.ay = ay
        agent.az = az

    def _agent_list(self, agents, **args):
        try:
            assert type(agents) == list
        except AssertionError:
            print 'msg agent_list :: agents is not a list'
            return
        GameStateGlobal.agentList.load_list(agents)

    def _agent_info(self, **args):
        agent_data = args.get('agent', None)
        if agent_data is None:
            print 'msg agent_info :: agent key missing'
            return

        GameStateGlobal.agentList.create(**agent_data)

    def _agent_update(self, **args):
        agent_data = args.get('agent', None)
        if agent_data is None:
            print 'msg agent_update :: agent key is missing'
            return

        GameStateGlobal.agentList.load_info(**agent_data)

    def _remove_agent(self, **args):
        id = args.get('id', None)
        if id is None:
            return
        GameStateGlobal.remove_agent(id)


class DatastoreMessageInterface(GenericMessageHandler):

    event_extensions = ['info', 'list', 'update', 'destroy', 'create'] # deprecate info for update
    name = ''
    store = None

    def __init__(self):
        self._load_default_events()
            
    def _load_default_events(self)
        for evex in self.event_extensions:
            event_name = '%s_%s' % (self.name, evex,)
            method_name = '_' + event_name
            klass_dict = self.__class__.__dict__
            if method_name not in klass_dict:
                method_name = '_default_%s' % (evex,)
                method = klass_dict[method_name]
            else:
                method = klass_dict[method_name]
            self.events[event_name] = method

    def _error_message(self, info_string, **msg):
        return 'msg %s :: %s' % (msg['cmd'], info_string,)

    def _default_info(self, **args):  # deprecate
        self._default_update(**args)

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

        
class MapMessageHandler(GenericMessageHandler):
    terrainMap = None
    mapChunkManager = None
    mapController = None

    events = {
        'chunk_list' : self._chunk_list,
        'map_chunk' : self._map_chunk,
        'set_map' : self._set_map,
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


from game_state import GameStateGlobal
from net_client import NetClientGlobal
from net_out import NetOut
from chat_client import ChatClientGlobal
from map_chunk_manager import MapChunkManagerGlobal
from map_controller import MapControllerGlobal
from input import InputGlobal
