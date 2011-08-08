#!/usr/bin/python

'''
Client network incoming
'''
import SDL.gl #for timer function

import json
import stats
#import struct
from opts import opts

class NetEventGlobal:
    messageHandler = None
    clientMessageHandler = None
    mapMessageHandler = None
    projectileMessageHandler = None
    agentMessageHandler = None
    playerMessageHandler = None
    miscMessageHandler = None
    gameModeMessageHandler = None
    itemMessageHandler = None
    
    @classmethod
    def init_0(cls):
        cls.messageHandler = MessageHandler()
        cls.clientMessageHandler = ClientMessageHandler()
        cls.playerMessageHandler = PlayerMessageHandler()
        cls.chatMessageHandler = ChatMessageHandler()
        cls.miscMessageHandler = MiscMessageHandler()

        cls.agentMessageHandler = AgentMessageHandler()
        cls.mapMessageHandler = MapMessageHandler()
        cls.projectileMessageHandler = ProjectileMessageHandler()

        cls.gameModeMessageHandler = GameModeMessageHandler()
        cls.itemMessageHandler = ItemMessageHandler()

    @classmethod
    def init_1(cls):
        MessageHandler.init()
        ClientMessageHandler.init()
        PlayerMessageHandler.init()
        ChatMessageHandler.init()

        AgentMessageHandler.init()
        MapMessageHandler.init()
        ProjectileMessageHandler.init()
        GameModeMessageHandler.init()
        ItemMessageHandler.init()
        
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
            self.json_events[cmd](**msg)
        else:
            print "Error, received command %s that client cannot handle" % (cmd,)
            assert False


class GenericMessageHandler:

    events = {}

    def register_events(self):
        NetEventGlobal.register_json_events(self.events)

    def _assign_events_to_methods(self):
        for event, name in self.events.items():
            ev = getattr(self, str(name), None)
            if ev is not None:
                self.events[event] = ev

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

            
class MiscMessageHandler(GenericMessageHandler):
    events = {
        'ping' : '_ping',
    }

    @classmethod
    def init(cls):
        pass

    def _ping(self, timestamp, **msg):
        stats.last_ping = SDL.gl.get_ticks() - timestamp
        #print "timestamp = %f" % (SDL.gl.get_ticks() - timestamp)

class MapMessageHandler(GenericMessageHandler):
    #terrainMap = None
    #mapChunkManager = None
    mapController = None

    events = {
        'chunk_list' : '_chunk_list',
        'map_chunk' : '_map_chunk',
        'set_map' : '_set_map',
    }

    @classmethod
    def init(cls):
        #cls.terrainMap = GameStateGlobal.terrainMap
        #cls.mapChunkManager = MapChunkManagerGlobal.mapChunkManager
        cls.mapController = MapControllerGlobal.mapController
        assert cls.mapController != None

    def _chunk_list(self, list, **msg):
        #print str(list)
        print "chunk list"
        self.mapController.process_chunk_list(list)
        #for chunk in list:
        #    (x,y,z,version ) = chunk

    def _map_chunk(self, datagram):
        #print "Map Chunk Received"
        (x,y,z) = terrainMap.set_packed_chunk(datagram)
        self.mapController.incoming_map_chunk(x,y,z)
        #self.mapChunkManager.set_map(x,y,z) #tells to redraw chunk

    def _set_map(self, list, **msg):
        for x,y,z,value in list:
            terrainMap.set(x,y,z,value)
            #self.mapChunkManager.set_map(x,y,z) #redraw chunk


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
        name = str(name)
        if not name:
            print 'msg identified :: name is empty'
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

    used_alt = False
    def _identify_fail(self, msg, **arg):
        # send system notification
        if self.used_alt:
            ChatClientGlobal.chatClient.system_notify('/identify_fail '+msg)
            ChatClientGlobal.chatClient.system_notify('/identify_fail Use /nick to set name.')
            # activate chat, insert /nick
            InputGlobal.enable_chat()
            ChatClientGlobal.chatClient.insert_string('/nick ')
        else:
            NetOut.sendMessage.identify(name=opts.alt_name)
            self.used_alt = True

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
        print args
        err_msg = None
        data = args.get(self.name, None)
        if data is None:
            err_msg = '%s key missing' % (self.name,)
        if err_msg is not None:
            print self._error_message(err_msg, **args)
            return
        print self.store
        obj = self.store.load_info(**data)
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


class PlayerMessageHandler(DatastoreMessageInterface):

    def __init__(self):
        self.name = 'player'
        self.store = GameStateGlobal.playerList
        self._bind_event('player_team', self._player_team)
        DatastoreMessageInterface.__init__(self)

    def _player_destroy(self, **args):
        id = self._default_destroy(**args)
        if id is not None:
            GameStateGlobal.remove_player(id)    # this method manages FK relationships

    def _player_team(self, **msg):
        err_msg = None
        try:
            pid = int(msg.get('id', None))
            player = GameStateGlobal.playerList[pid]
        except ValueError:
            err_msg = 'player id invalid'
        except TypeError:
            err_msg = 'player id missing'

        if player is None:
            err_msg = 'player %d unknown' % (pid,)
            NetOut.sendMessage.request_player(pid)

        try:
            team_id = int(msg.get('team', None))
            team = GameStateGlobal.teamList[team_id]
        except ValueError:
            err_msg = 'team id invalid'
        except TypeError:
            err_msg = 'team id missing'
        except (KeyError, IndexError):
            err_msg = 'team %d sunknown' % (team_id,)

        if err_msg is not None:
            print 'msg player_team :: %s' % (err_msg,)
            return

        GameStateGlobal.game.player_join_team(player, team)

    def _player_list(self, **args):
        self._default_list(**args)
        if GameStateGlobal.game is not None:
            GameStateGlobal.game.update_players()
        

# agent messages needs to be updated
# there is no agent_create, and agent_destroy is called remove_agent
class AgentMessageHandler(DatastoreMessageInterface):

    def __init__(self):
        self.name = 'agent'
        self.store = GameStateGlobal.agentList
        self._bind_event('agent_position', self._agent_position)
        self._bind_event('agent_button_state', self._agent_button_state)
        self._bind_event('agent_control_state', self._agent_control_state)
        self._bind_event('agent_angle', self._agent_angle)
        DatastoreMessageInterface.__init__(self)

    def _agent_position(self, **args):
        pos = args.get('pos', None)
        id = args.get('id', None)
        tick = args.get('tick', None)
        if None in (pos, id, tick,):
            print 'agent_position, missing keys'
            print args
            return

        agent = GameStateGlobal.agentList[id]
        if agent is None: # agent not found, request agent
            NetOut.sendMessage.request_agent(id)
            return

        if agent.you:
            return
        #agent.pos(pos)
        agent.state = pos
        #print agent.state

    def _agent_angle(self, **msg):
        err_msg = None
        try:
            angle = msg['angle']
        except KeyError:
            err_msg = 'msg agent_angle :: angle missing'
        try:
            agent_id = msg['id']
        except KeyError:
            err_msg = 'msg agent_angle :: agent id missing'
        try:
            tick = msg['tick']
        except KeyError:
            err_msg = 'msg agent_angle :: tick missing'

        try:
            agent = GameStateGlobal.agentList[agent_id]
        except KeyError:
            err_msg = 'msg agent_angle :: agent %s does not exist' % (str(agent_id),)

        if agent is None:
            print 'agent_angle msg :: agent %s is None' % (agent_id,)
            print 'AgentList: %s' % (str(GameStateGlobal.agentList,))

        if err_msg is not None:
            print err_msg
            return

        if agent.you:
            return
        print 'setting angle %s to agent %s' % (angle, agent.id,)
        agent.set_angle(angle)

    #deprecated
    def _agent_control_state(self, **msg):
        print 'received deprecated agent_control_state msg. ignoring'
        return
        err_msg = None
        try:
            state = msg['state']
        except KeyError:
            err_msg = 'msg agent_control_state :: state missing'
        try:
            angle = msg['angle']
        except KeyError:
            err_msg = 'msg agent_control_state :: angle missing'
        try:
            agent_id = msg['id']
        except KeyError:
            err_msg = 'msg agent_control_state :: agent id missing'
        try:
            tick = msg['tick']
        except KeyError:
            err_msg = 'msg agent_control_state :: tick missing'

        try:
            agent = GameStateGlobal.agentList[agent_id]
        except KeyError:
            err_msg = 'msg agent_control_state :: agent %s does not exist' % (str(agent_id),)

        if agent is None:
            print 'Agent_control_state msg :: agent %s is None' % (agent_id,)
            print 'AgentList: %s' % (str(GameStateGlobal.agentList,))

        if err_msg is not None:
            print err_msg
            return

        if agent.you:
            return

        agent.set_control_state(state, angle, tick)


    def _agent_button_state(self, **msg):
        err_msg = None
        try:
            buttons = msg['buttons']
        except KeyError:
            err_msg = 'msg agent_button_state :: buttons key is missing'
        btn_size = 6
        btn_len = len(buttons)
        if btn_size != btn_len:
            err_msg = 'msg agent_button_state :: buttons array is wrong size (is %d should be %d)' % (btn_len, btn_size)
        try:
            tick = msg['tick']
        except KeyError:
            err_msg = 'msg agent_button_state :: tick is missing'
        try:
            agent_id = msg['id']
        except KeyError:
            err_msg = 'msg agent_button_state :: aid is missing'
        try:
            agent = GameStateGlobal.agentList[agent_id]
        except KeyError:
            err_msg = 'msg agent_button state :: agent %s not found' % (agent_id,)
        if err_msg is not None:
            print err_msg
            return

        if agent.you:
            return

        if tick > agent.last_button_tick:
            agent.last_button_tick = tick
            agent.button_state = buttons

        ctrl_state = agent.compute_state()
        agent.set_control_state(ctrl_state)

    def _agent_destroy(self, **args):
        id = self._default_destroy(**args)
        if id is not None:
            GameStateGlobal.remove_agent(id)    # this method manages FK relationships


class WeaponMessageHandler(DatastoreMessageInterface):

    def __init__(self):
        self.name = 'weapon'
        self.store = GameStateGlobal.weaponList
        DatastoreMessageInterface.__init__(self)

    def _weapon_destroy(self, **args):
        id = self._default_destroy(**args)
        if id is not None:
            GameStateGlobal.remove_weapon(id)


class ItemMessageHandler(DatastoreMessageInterface):

    def __init__(self):
        self.name = 'item'
        self.store = GameStateGlobal.itemList
        DatastoreMessageInterface.__init__(self)

    def _object_destroy(self, **args):
        id = self._default_destroy(**args)
        if id is not None:
            GameStateGlobal.remove_item(id)


class ProjectileMessageHandler(DatastoreMessageInterface):

    def __init__(self):
        self.name = 'projectile'
        self.store = GameStateGlobal.projectileList
        self._bind_event('hitscan', self._hitscan)
        DatastoreMessageInterface.__init__(self)

    def _hitscan(self, **msg):
        err_msg = None
        try:
            agent_id = msg['aid']
            if agent_id == GameStateGlobal.agent.id: # ignore, you fired this
                return
        except KeyError:
            err_msg = 'msg hitscan :: aid missing'
        try:
            agent = GameStateGlobal.agentList[agent_id]
        except KeyError:
            err_msg = 'msg hitscan :: agent %s does not exist' % (agent_id,)
        try:
            target = msg['target']
        except KeyError:
            err_msg = 'msg hitscan :: target missing'
        try:
            type = target['type']
        except KeyError:
            err_msg = 'msg hitscan :: target type missing'
        try:
            loc = target['loc']
            if type == 'block' or type == 'empty':
                assert len(loc) == 3
            elif type == 'agent':
                assert len(loc) == 2
        except KeyError:
            err_msg = 'msg hitscan :: target location missing'
        except TypeError:
            err_msg = 'msg hitscan :: target block location not iterable'
        except AssertionError:
            err_msg = 'msg hitscan :: target location wrong length'
        try:
            weapon_type = msg['wtype']
        except KeyError:
            err_msg = 'msg hitscan :: wtype missing'

        # look up projectile type
        ptype = weapon_dat[weapon_type]['projectile_type']
        firing_weapon = agent.weapons.has(weapon_type)

        # look up spatial coordinates of target
        if type == 'block':
            # later, adjust this so that the end is at the corrent surface point of the block
            anim = lambda: firing_weapon.animation(agent=agent, target=loc).play()
        elif type == 'agent':
            target_agent_id, body_part_id = loc
            target = GameStateGlobal.agentList[target_agent_id]
            if target is None:
                print 'msg hitscan :: target agent %s does not exist' % (target_agent_id,)
                print 'msg :: %s' % (msg,)
                return
            else:
                target = target.pos()
                anim = lambda: firing_weapon.animation(agent=agent, target=target).play()
        elif type == 'empty':
            # special mode; in this case, loc is a unit vector
            # call different animation
            anim = lambda: firing_weapon.animation(agent=agent, vector=loc).play()

        if err_msg is not None:
            print self._error_message(err_msg, **args)
            return

        if firing_weapon:
            anim()
        # look up agent origin
        # animate
        #print 'animating projectile_type %d to wherever target %s %s is' % (ptype, type, loc,)


class GameModeMessageHandler(DatastoreMessageInterface):

    def __init__(self):
        self.name = 'teams'
        self.store = GameStateGlobal.teamList
        self._bind_event('game_mode', '_game_mode')
        self._bind_event('teams', '_teams')
        DatastoreMessageInterface.__init__(self)

    def _game_mode(self, **msg):
        try:
            mode = msg['mode']
        except KeyError:
            print 'msg game_mode :: mode missing'
            return
        teams = None
        try:
            teams = int(msg['teams'])
        except ValueError:
            print 'msg game_mode :: teams number invalid'
            return
        except KeyError:
            pass

        if 'teams_list' in msg:
            self._teams(**msg)
            
        if teams is None:
            GameStateGlobal.start_game_mode(mode)
        else:
            GameStateGlobal.start_game_mode(mode, teams=teams)
            
    def _teams(self, **msg):
        print 'teams'
        print msg
        self._default_list(**msg)
        print GameStateGlobal.teamList

    def _player_team(self, **msg):
        err_msg = None
        try:
            pid = int(msg.get('id', None))
            player = GameStateGlobal.playerList[pid]
        except TypeError:
            err_msg = 'player id missing'
        except ValueError:
            err_msg = 'player id invalid'
        except KeyError:
            err_msg = 'player %d unknown' % (pid,)

        try:
            tid = int(msg.get('team', None))
            team = GameStateGlobal.teamList[tid]
        except TypeError:
            err_msg = 'team missing'
        except ValueError:
            err_msg = 'team invalid'
        except KeyError:
            err_msg = 'team %d unknown' % (pid,)

        if err_msg is not None:
            print 'msg player_team :: %s' % (err_msg,)
            return

        GameStateGlobal.game.player_join_team(player, team)


from game_state import GameStateGlobal
from net_client import NetClientGlobal
from net_out import NetOut
from chat_client import ChatClientGlobal
#from map_chunk_manager import MapChunkManagerGlobal
from map_controller import MapControllerGlobal
from input import InputGlobal

import cube_lib.terrain_map as terrainMap
from weapons import weapon_dat

import animations
