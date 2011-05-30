#!/usr/bin/python

'''
Client network incoming
'''

import simplejson as json
#import struct

class NetEventGlobal:
    messageHandler = None
    mapMessageHandler = None
    @classmethod
    def init_0(self):
        self.messageHandler = MessageHandler()  ##MAY CAUSE ERRORS?
        self.mapMessageHandler = MapMessageHandler()
    @classmethod
    def init_1(self):
        pass
        MessageHandler.init()
        MapMessageHandler.init()

class MessageHandler:
    player = None #move this somewhere else
    @classmethod
    def init(self):
        self.player = GameStateGlobal.player
        self.agent = GameStateGlobal.agent
        assert self.player != None
        assert self.agent != None
    def __init__(self):
        pass

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

        # agents
        if cmd == 'agent_position':
            self._agent_position(**msg)
        elif cmd == 'agent_update':
            self._agent_update(**msg)

        # projectiles
        elif cmd == ' projectile_create':
            self._create_projectile(**msg)
        elif cmd == 'projectile_update':
            self._update_projectile(**msg)
        elif cmd == 'projectile_destroy':
            self._destroy_projectile(**msg)

        # initial settings
        elif cmd == 'client_id':
            if self._set_client_id(**msg):
                NetOut.sendMessage.identify()
            else:
                NetOut.sendMessage.request_client_id()
        elif cmd == 'identify_fail':
            msg = msg.get('msg', '')
            msg = 'Identification failed. %s' % (msg,)
            print msg
            # send system notification
            ChatClientGlobal.chatClient.system_notify('/identify_fail '+msg)
            ChatClientGlobal.chatClient.system_notify('/identify_fail Use /nick to set name.')
            # activate chat, insert /nick
            InputGlobal.enable_chat()
            ChatClientGlobal.chatClient.insert_string('/nick ')

        elif cmd == 'identified':
            self._on_identify(**msg)
            
        #map events
        elif cmd == 'chunk_list':
            NetEventGlobal.mapMessageHandler._chunk_list(**msg)
            #print "Chunk List Received"
            #print str(msg['list'])
        elif cmd == 'set_map':
            NetEventGlobal.mapMessageHandler._set_map(**msg)

        elif cmd == 'client_quit':
            id = msg.get('id', None)
            if id is None:
                return
            GameStateGlobal.client_quit(id)

        elif cmd == 'player_list':
            try:
                players = msg['players']
                assert type(players) == list
            except KeyError:
                print 'msg player_list :: players key missing'
                return
            except AssertionError:
                print 'msg player_list :: players is not a list'
                return
            GameStateGlobal.load_player_list(players)
        elif cmd == 'player_info':
            if not self._update_player(**msg):
                return
        elif cmd == 'player_update':
            if not self._update_player(**msg):
                return
        elif cmd == 'remove_player':
            id = msg.get('id', None)
            if id is None:
                return
            GameStateGlobal.remove_player(id)
        elif cmd == 'remove_agent':
            id = msg.get('id', None)
            if id is None:
                return
            GameStateGlobal.remove_agent(id)
            
        elif cmd == 'chat':
            ChatClientGlobal.chatClient.receive(msg)
        else:
            print "JSON message type unrecognized"

    def _update_player(self, **msg):
        try:
            player = msg['player']
            assert type(player) == dict
        except KeyError:
            print 'msg player_info :: player key missing'
            return False
        except AssertionError:
            print 'msg player_info :: player is not a dict'
            return False
        GameStateGlobal.load_player_info(**player)
        return True

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

    def _agent_update(self, **args):
        agent_data = args.get('agent', None)
        if agent is None:
            print 'msg agent_update :: agent key is missing'

        try:
            agent_id = int(agent_data.get('id', None))
        except ValueError:
            print 'msg agent_update :: agent dict id is missing or invalid'
            return

        GameStateGlobal.agentList[agent_id].update_info(**agent_data)
        

    def _create_projectile(**args):
        projectile = args.get('projectile', None)
        if projectile is None:
            print 'msg create_projectile :: missing projectile'
            return
        GameStateGlobal.projectileList.create(**projectile)

    def _update_projectile(**args):
        projectile_data = args.get('projectile', None)
        if projectile is None:
            print 'msg update_projectile :: missing projectile'
            return
        try:
            projectile_id = int(projectile_data.get('id', None))
            projectile = GameStateGlobal.projectileList[projectile_id]
            projectile.update(**projectile_data)
        except TypeError:
            print 'msg update_projectile :: Projectile id missing'
        except ValueError:
            print 'msg projectile_update :: projectile id invalid'
        except KeyError:
            print 'msg update_projectile :: projectile not found'

    def _destroy_projectile(**args):
        try:
            id = int(args.get('id', None))
            projectile = GameStateGlobal.projectileList[id]
            GameStateGlobal.projectileList.destroy(projectile)
        except TypeError:
            print 'msg projectile_destroy :: projectile id missing'
        except ValueError:
            print 'msg projectile_destroy :: projectile id invalid'
        except KeyError:
            print 'msg projectile_destroy :: projectile not found'

    def _set_client_id(self, **msg):
        id = msg.get('id', None)
        if id is None:
            print '_register msg missing id'
            return False
        print "Received Client Id: %s" % (id,)
        NetClientGlobal.client_id = id
        return True

    def _on_identify(self, **msg):
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

        player = GameStateGlobal.update_info(player)
        if player.you:
            GameStateGlobal.playerList.identify(player)
        return True

class MapMessageHandler:
    terrainMap = None
    mapChunkManager = None
    mapController = None
    @classmethod
    def init(self):
        self.terrainMap = GameStateGlobal.terrainMap
        self.mapChunkManager = MapChunkManagerGlobal.mapChunkManager
        self.mapController = MapControllerGlobal.mapController
        assert self.mapController != None
    def __init__(self):
        pass

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

    def _set_map(self, value, **msg):
        (x,y,z,value) = value
        self.terrainMap.set(x,y,z,value)
        self.mapChunkManager.set_map(x,y,z) #redraw chunk


from game_state import GameStateGlobal
from net_client import NetClientGlobal
from net_out import NetOut
from chat_client import ChatClientGlobal
from map_chunk_manager import MapChunkManagerGlobal
from map_controller import MapControllerGlobal
from input import InputGlobal
