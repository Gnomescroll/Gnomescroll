'''
Incoming net events
'''

class NetEvent:

    messageHandler = None
    adminMessageHandler = None
    miscMessageHandler = None
    agentMessageHandler = None
    mapMessageHandler = None
    playerMessageHandler = None
    weaponMessageHandler = None
    itemMessageHandler = None
    gameModeMessageHandler = None
    projectileMessageHandler = None
    chatMessageHandler = None

    @classmethod
    def init_0(cls):
        cls.messageHandler = MessageHandler()
        cls.adminMessageHandler = AdminMessageHandler()
        cls.agentMessageHandler = AgentMessageHandler()
        cls.miscMessageHandler = MiscMessageHandler()
        cls.mapMessageHandler = MapMessageHandler()
        cls.playerMessageHandler = PlayerMessageHandler()
        cls.weaponMessageHandler = WeaponMessageHandler()
        cls.itemMessageHandler = ItemMessageHandler()
        cls.gameModeMessageHandler = GameModeMessageHandler()
        cls.projectileMessageHandler = ProjectileMessageHandler()
        cls.chatMessageHandler = ChatMessageHandler()
        
    @classmethod
    def init_1(cls):
        pass
        
    @classmethod
    def register_json_events(cls, events, interface=None):
        for name, function in events.items():
            if function is None and interface is not None:
                function = getattr(interface, name)
            cls.messageHandler.json_events[name] = function

from net_server import NetServer
from net_out import NetOut
from game_state import GameStateGlobal
from chat_server import ChatServer

# routes messages by msg.cmd
class MessageHandler:

    def __init__(self):
        self.json_events = {} #map strings to functions

    def process_json(self, msg, connection):
        cmd = msg.get('cmd', None)
        #print "MessageHandler.process_json: " + str(msg)
        if cmd is None:
            print 'Warning: message missing cmd. message: %s' % msg
            return
        #use json_events when possible
        if self.json_events.has_key(cmd):
            self.json_events[cmd](connection, **msg)
        else:
            print "MessageHandler.process_json: cmd unknown = %s" % (str(msg),)
        #elif cmd == 'ping':
            #_ping(connection, **msg)
        # game state
        #elif cmd == 'create_agent':
            #GameStateGlobal.agentList.create(**msg)
        #elif cmd == 'agent_control_state':
            #self.agent_control_state(connection.id, **msg)
        #elif cmd == 'agent_button_state':
            #self.agent_button_state(connection.id, **msg)
        ##elif cmd == 'agent_position':
            ##self.agent_position(connection.id, **msg)
        #elif cmd == 'agent_angle':
            #self.agent_angle(connection.id, **msg)
        #elif cmd == 'request_agent':
            #self.request_agent(connection, **msg)
        #elif cmd == 'request_player':
            #self.request_player(connection, **msg)
            
        #elif cmd == 'request_team':
            #self.request_team(connection, **msg)
        #elif cmd == 'request_item':
            #self.request_item(connection, **msg)
        #elif cmd == 'request_weapon':
            #self.request_weapon(connection, **msg)
        #elif cmd == 'request_projectile':
            #self.request_projectile(connection, **msg)

        #elif cmd == 'fire_projectile':
            #self.fire_projectile(connection.id, **msg)
        #elif cmd == 'reload_weapon':
            #self.reload_weapon(connection.id, **msg)
        #elif cmd == 'change_weapon':
            #self.change_weapon(connection.id, **msg)
        #elif cmd == 'drop_weapon':
            #self.drop_weapon(connection.id, **msg)
        #elif cmd == 'throw_grenade':
            #self.throw_grenade(connection.id, **msg)

        ##elif cmd == 'pickup_item':
            ##self.pickup_item(connection.id, **msg)
        #elif cmd == 'drop_item':
            #self.drop_item(connection.id, **msg)
        #elif cmd == 'near_item':
            #self.near_item(connection.id, **msg)

        #elif cmd == 'set_block':
            #self.set_block(connection, **msg)
        #elif cmd == 'hit_block':
            #self.hit_block(connection.id, **msg)

        #elif cmd == 'hitscan':
            #self.hitscan(connection.id, **msg)

        #elif cmd == 'join_team':
            #self.join_team(connection.id, **msg)

        #chat
        #elif cmd == 'chat':
            #ChatServer.chat.received(msg, connection)
        #elif cmd == 'subscribe':
            #ChatServer.chat.client_subscribe(msg, connection)
        #elif cmd == 'unsubscribe':
            #ChatServer.chat.client_unsubscribe(msg, connection)

        # setup
        #elif cmd == 'identify': #Setup client connection
            #name = msg.get('name', None)
            #if name is None:
                #print 'msg identify - name is missing'
                #return
            #connection.identify(name)
        #elif cmd == 'request_client_id':
            #connection.send_client_id()
        #misc
        #elif cmd == 'ping'
        #    self.ping(msg, connection)
        # map
        #elif cmd == 'request_chunk_list':
            #self.send_chunk_list(msg, connection)
        #elif cmd == 'request_chunk':
            #self.request_chunk(msg, connection)
        #else:
            #print "MessageHandler.process_json: cmd unknown = %s" % (str(msg),)


'''
Common Errors
'''

class ErrorNames:

    def agent_ownership(p, a):
        return 'player %d does not own agent %d' % (p.id, a.id,)

    def is_viewer(msg):
        return 'ignoring viewing agent; cmd %s' % (msg['cmd'],)

    def key_missing(key):
        return '%s missing' % (key,)

    def key_invalid(key):
        return '%s invalid' % (key,)

    def key_invalid_or_missing(key):
        return '%s invalid or missing' % (key,)

    def wrong_size(key):
        return '%s of wrong size' % (key,)

    def not_iterable(key):
        return '%s is not iterable' % (key,)

    def thing_unknown(name, thing_id):
        return '%s %d unknown' % (name, thing_id,)

    def agent_unknown(agent_id):
        return thing_unknown('agent', agent_id)

    def player_has_no_agent(player):
        return 'player %d has no agent' % (player.id,)

err = ErrorNames()

        
'''
Processors, for common message items/keys
'''

class ProcessedAgentMessage:
    __slots__ = ['error', 'agent']
    def __init__(self, error=None, agent=None):
        self.error = error
        self.agent = agent

class Processors:

# for msgs that send agent id and want it extracted and checked against player
    def agent_key(player, msg, key='id'):
        error, agent, agent_id = None, None, None
        try:
            agent_id = int(msg.get(key, None))
            agent = GameStateGlobal.agentList[agent_id]
            if agent.team.is_viewers():
                err_msg = err.is_viewer(msg)
            if not player.owns(agent):
                err_msg = err.agent_ownership(player, agent)
        except TypeError:
            err_msg = err.key_missing(key)
        except ValueError:
            err_msg = err.key_invalid(key)
        except KeyError:
            err_msg = err.agent_unknown(agent_id)

        return ProcessedAgentMessage(err_msg, agent)

# for msgs that assume agent is player's agent
    def agent(player, msg):
        err_msg, agent = None, None
        try:
            agent = player.agent
            if agent.team.is_viewers():
                err_msg = err.is_viewer(msg)
        except AttributeError:
            err_msg = err.player_has_no_agent(player)
        return ProcessedAgentMessage(err_msg, agent)

processor = Processors()


'''
    Decorators
    Use @logError('msg_name') first
'''

def extractPlayer(f):
    def wrapped(self, conn, *args, **kwargs):
        try:
            player = GameStateGlobal.playerList.client(conn.id)
        except KeyError:
            err = 'could not find player for client %s' % (conn.id,)
            return err
        return f(self, player, *args, **kwargs)
    return wrapped

def logError(msg_name):
    def outer(f):
        def wrapped(self, conn, *args, **kwargs):
            err = f(self, conn, *args, **kwargs)
            if err is not None:
                print '%s :: %s' % (msg_name, err,)
        return wrapped
    return outer


'''
Message Handlers
'''

class GenericMessageHandler:

    def __init__(self):
        self.register_events()

    def _events(self):
        return {}
        
    def register_events(self):
        NetEvent.register_json_events(self.events(), self)

        
# handler for admin msgs
class AdminMessageHandler(GenericMessageHandler):

    def events(self):
        return {
            'set_map' : self._set_map,
        }

    def _set_map(self, list, **msg):
        terrainMap = GameStateGlobal.terrainMap
        for x,y,z,value in list:
            terrainMap.set(x,y,z,value)
        NetOut.event.set_map(list)


class AgentMessageHandler(GenericMessageHandler):

    def _events(self):
        return {
            'request_agent'     :   self.request_agent,
            'agent_position'    :   self.agent_position,
            'agent_button_state':   self.agent_button_state,
            'agent_angle'       :   self.agent_angle,
            'create_agent'      :   self.create_agent,
        }

    @logError('request_agent')
    def request_agent(self, connection, **msg):
        if 'id' not in msg:
            return err.key_missing('id')
        agent_id = msg['id']
        connection.sendMessage.send_agent(agent_id)

    @logError('create_agent')
    def create_agent(self, connection, **msg):
        print 'Received message CREATE_AGENT. Why????'
        GameStateGlobal.agentList.create(**msg)

    @logError('agent_position')
    @extractPlayer
    def agent_position(self, player, **msg):
        a = processor.agent_key(player, msg)
        if a.error:
            return a.error
        agent = a.agent

        err_msg = None
        try:
            pos = msg['pos']
        except KeyError:
            err_msg = err.key_missing('pos')
        if err_msg: return err_msg

        agent.state = pos
        NetOut.event.agent_position(agent)

    @logError('agent_button_state')
    @extractPlayer
    def agent_button_state(self, player, **msg):
        a = processor.agent_key(player, msg)
        if a.error:
            return a.error
        agent = a.agent

        err_msg = None
        try:
            buttons = msg['buttons']
        except KeyError:
            err_msg = err.key_missing('buttons')

        btn_len = len(buttons)
        btn_size = 6
        if btn_len != btn_size:
            err_msg = err.wrong_size('buttons')

        if err_msg: return err_msg

        old_buttons = agent.button_state
        agent.button_state = buttons

        #forward msg
        if old_buttons != buttons:
            NetOut.event.agent_button_state(agent)
            ctrl_state = agent.compute_state()
            agent.set_control_state(ctrl_state)

    @logError('agent_angle')
    @extractPlayer
    def agent_angle(self, player, **msg):
        err_msg = None
        a = processor.agent_key(player, msg)
        if a.error:
            return a.error
        agent = a.agent

        tick = msg.get('tick', None)
        if tick is None:
            err_msg = err.key_missing('tick')

        try:
            angle = msg['angle']
            angle = list(angle)
            assert len(angle) == 2
        except KeyError:
            err_msg = err.key_missing('angle')
        except TypeError:
            err_msg = err.not_iterable('angle')
        except AssertionError:
            err_msg = err.wrong_size('angle')

        if err_msg: return err_msg
    
        agent.set_angle(angle)


class PlayerMessageHandler:

    def events(self):
        return {
            'request_player'    :   self.request_player,
        }

    @logError('request_player')
    def request_player(self, connection, **msg):
        if 'id' not in msg:
            return err.key_missing('id')
        connection.sendMessage.send_player(msg['id'])


class ProjectileMessageHandler:

    def events(self):
        return {
            'request_projectile':   self.request_projectile,
            'fire_projectile'   :   self.fire_projectile,
            'throw_grenade' : self.throw_grenade,
        }

    @logError('request_projectile')
    def request_projectile(self, conn, **msg):
        if 'id' not in msg:
            return err.key_missing('id')
        conn.sendMessage.send_projectile(msg['id'])

    @logError('throw_grenade')
    @extractPlayer
    def throw_grenade(self, player, **msg):
        a = processor.agent_key(player, msg, 'aid')
        if a.error:
            return a.error
        agent = a.agent

        err_msg = None
        try:
            vector = msg['vector']
            assert len(vector) == 3
        except KeyError:
            err_msg = 'msg throw_grenade :: vector missing'
        except AssertionError:
            err_msg = 'msg throw_grenade :: vector of wrong size'

        if err_msg is not None:
            return err_msg

        weapon = agent.active_weapon()
        if weapon.fire_command == 'throw_grenade' and weapon.fire():
            agent.throw_grenade(vector)

    @logError('fire_projectile')
    @extractPlayer
    def fire_projectile(self, player, **msg):
        err_msg = None
        a = processor.agent_key(player, msg, 'aid')
        if a.error:
            return a.error
        agent = a.agent

        try:
            pos = msg['pos']
            assert len(pos) == 3
        except KeyError:
            err_msg = err.key_missing('pos')
        except AssertionError:
            err_msg = err.wrong_size('pos')

        try:
            vec = msg['vec']
            assert len(vec) == 3
        except KeyError:
            err_msg = err.key_missing('vec (direction)')
        except AssertionError:
            err_msg = err.wrong_size('vec')

        if err_msg: return err_msg

        weapon = agent.active_weapon()
        if weapon.fire_command == 'fire_projectile' and weapon.fire():
            agent.fire_projectile(pos=pos, direction=vec)

    
class WeaponMessageHandler:

    def events(self):
        return {
            'request_weapon'    :   self.request_weapon,
            'reload_weapon'     :   self.reload_weapon,
            'hitscan'           :   self.hitscan,
            'drop_weapon'       :   self.drop_weapon,
            'change_weapon'     :   self.change_weapon,
        }

    @logError('request_weapon')
    def request_weapon(self, conn, **msg):
        if 'id' not in msg:
            return 'id missing'
        conn.sendMessage.send_weapon(msg['id'])

    @logError('reload_weapon')
    @extractPlayer
    def reload_weapon(self, player, **msg):
        err_msg = None
        a = processor.agent_key(player, msg, 'aid')
        if a.error:
            return a.error
        agent = a.agent

        try:
            weapon_type = int(msg.get('weapon', None))
        except TypeError:
            err_msg = 'weapon missing'
        except ValueError:
            err_msg = 'weapon invalid'

        if err_msg: return err_msg
            
        try:
            weapon_index = [weapon.type for weapon in agent.weapons].index(weapon_type)
        except ValueError:
            err_msg = 'weapon unknown to agent'

        if err_msg: return err_msg
            
        weapon = agent.weapons[weapon_index]
        if weapon.reload():
            NetOut.event.agent_update(agent, 'weapons')

    @logError('hitscan')
    @extractPlayer
    def hitscan(self, player, **msg):
        err_msg = None
        
        a = processor.agent(player, msg)
        if a.error:
            return a.error
        firing_agent = a.agent
            
        weapon = firing_agent.active_weapon()
        if not weapon.hitscan:
            err_msg = 'Client sent hitscan message for non-hitscan weapon'

        try:
            target = msg['target']
        except KeyError:
            err_msg = err.key_missing('target')

        if err_msg: return err_msg

        try:
            type = target['type']
        except KeyError:
            err_msg = err.key_missing('target type')

        try:
            loc = target['loc']
            if type == 'block' or type == 'empty':
                assert len(loc) == 3
            elif type == 'agent':
                assert len(loc) == 2
        except KeyError:
            err_msg = err.key_missing('target loc')
        except TypeError:
            err_msg = err.not_iterable('target loc')
        except AssertionError:
            err_msg = err.wrong_size('target loc')

        if err_msg: return err_msg

        # add agent/projectile information to packet and forward
        if not weapon.fire():
            return
        NetOut.event.hitscan(target, firing_agent.id, weapon.type)
        print 'Hitscan target type %s' % (type,)
        # apply damage
        if type == 'block':
            pass
        elif type == 'agent':
            target_aid, body_part_id = loc
            try:
                target_agent = GameStateGlobal.agentList[target_aid]
            except KeyError:
                return 'target agent does not exist'
            # improve damage calculation later
            target_agent.take_damage(weapon.base_damage, firing_agent.owner)

    @logError('change_weapon')
    @extractPlayer
    def change_weapon(self, player, **msg):
        err_msg = None
        a = processor.agent_key(player, msg, 'aid')
        if a.error:
            return a.error
        agent = a.agent

        try:
            active_weapon = msg['windex']
        except KeyError:
            err_msg = err.key_missing('windex (active_weapon)')

        if err_msg: return err_msg

        if active_weapon == -1: # json doesnt have None, but None is a valid input; careful, -1 is a valid index.
            active_weapon = None

        agent.set_active_weapon(active_weapon)

    @logError('drop_weapon')
    @extractPlayer
    def drop_weapon(self, player, **msg):
        err_msg = None
        a = processor.agent_key(player, msg, 'aid')
        if a.error:
            return a.error
        agent = a.agent

        try:
            weapon_id = msg['wid']
        except KeyError:
            err_msg = err.key_missing('wid')

        if err_msg: return err_msg

        agent.drop_weapon(weapon_id, by_id=True)


class ItemMessageHandler:

    def events(self):
        return {
            'request_item'  :   self.request_item,
            'near_item'     :   self.near_item,
            'pickup_item'   :   self.pickup_item,
            'drop_item'     :   self.drop_item,
        }

    @logError('request_item')
    def request_item(self, conn, **msg):
        if 'id' not in msg:
            return 'id missing'
        conn.sendMessage.send_item(msg['id'])

    @logError('pickup_item')
    @extractPlayer
    def pickup_item(self, player, **msg):
        err_msg = None
        a = processor.agent_key(player, msg, 'aid')
        if a.error:
            return a.error
        agent = a.agent

        try:
            item_id = int(msg.get('iid', None))
            item = GameStateGlobal.itemList[item_id]
        except TypeError:
            err_msg = 'msg pickup_item :: iid missing'
        except ValueError:
            err_msg = 'msg pickup_item :: iid invalid'
        except KeyError:
            err_msg = 'msg pickup_item :: object %s unknown' % (item_id,)
            
        slot = None
        try:
            slot = int(msg['slot'])
        except ValueError:
            err_msg = 'msg drop_item :: slot invalid'
        except KeyError:
            pass

        if err_msg is not None:
            return err_msg

        if agent.near_item(item):
            print 'agent picking up %s' % (item,)
            agent.pickup_item(item, slot)

    @logError('near_item')
    @extractPlayer
    def near_item(self, player, **msg):
        err_msg = None
        a = processor.agent_key(player, msg, 'aid')
        if a.error:
            return a.error
        agent = a.agent

        try:
            iid = msg['iid']
        except KeyError:
            err_msg = 'item id missing'

        if err_msg: return err_msg

        try:
            item = GameStateGlobal.itemList[iid]
        except KeyError:
            err_msg = 'item %s unknown' % (iid,)

        if err_msg: return err_msg

        if agent.near_item(item) and hasattr(item, 'agent_nearby'):
            item.agent_nearby(agent)

    @logError('drop_item')
    @extractPlayer
    def drop_item(self, player, **msg):
        err_msg = None
        a = processor.agent_key(player, msg, 'aid')
        if a.error:
            return a.error
        agent = a.agent

        try:
            item_id = int(msg.get('iid', None))
            item = GameStateGlobal.itemList[item_id]
        except TypeError:
            err_msg = err.key_missing('iid')
        except ValueError:
            err_msg = err.key_invalid('iid')
        except KeyError:
            err_msg = err.thing_unknown('item', item_id)

        if err_msg: return err_msg

        agent.drop_item(item)


class GameModeMessageHandler:

    def events(self):
        return {
            'join_team' :   self.join_team,
            'request_team': self.request_team,
        }

    @logError('join_team')
    @extractPlayer
    def join_team(self, player, **msg):
        err_msg = None
        try:
            team_id = int(msg.get('team', None))
            team = GameStateGlobal.teamList[team_id]
        except TypeError:
            err_msg = 'msg join_team :: team key missing'
        except ValueError:
            err_msg = 'msg join_team :: team invalid'
        except KeyError:
            err_msg = 'msg join_team :: team does not exist'

        if err_msg is not None:
            return err_msg
            
        GameStateGlobal.game.player_join_team(player, team)

    @logError('request_team')
    def request_team(self, conn, **msg):
        if 'id' not in msg:
            return err.no_id
        conn.sendMessage.send_team(msg['id'])



class MiscMessageHandler:

    def events(self):
        return {
            'ping'  :   self.ping,
            'identify': self.identify,
            'request_client_id':    self.request_client_id,
        }

    @logError('ping')
    def ping(self, connection, **msg):
        try:
            ts = msg['timestamp']
        except KeyError:
            return err.key_missing('timestamp')
        connection.sendMessage.ping(ts)

    @logError('identify')
    def identify(self, conn, **msg):
        name = msg.get('name', None)
        if name is None:
            return err.key_missing('name')
        conn.identify(name)

    def request_client_id(self, conn, **msg):
        conn.send_client_id()
        

class MapMessageHandler:

    def events(self):
        return {
            'request_chunk' :   self.request_chunk,
            'request_chunk_list':  self.send_chunk_list,
            'hit_block'     :   self.hit_block,
            'set_block'     :   self.set_block,
        }

    @logError('request_chunk')
    def request_chunk(self, connection, **msg):
        err_msg = None
        try:
            x,y,z = msg['value']
        except KeyError:
            err_msg = '"value" missing'
        except ValueError:
            err_msg = '"value" must be a 3 tuple'

        if err_msg: return err_msg
        connection.sendMessage.send_chunk(x,y,z)

    @logError('send_chunk_list')
    def send_chunk_list(self, connection, **msg):
        connection.sendMessage.send_chunk_list()

    @logError('hit_block')
    @extractPlayer
    def hit_block(self, player, **msg):
        err_msg = None
        a = processor.agent_key(player, msg, 'aid')
        if a.error:
            return a.error
        agent = a.agent

        try:
            block_position = msg['pos']
            block_position = list(block_position)
            assert len(block_position) == 3
        except KeyError:
            err_msg = err.key_missing('pos')
        except ValueError:
            err_msg = err.not_iterable('pos')
        except AssertionError:
            err_msg = err.wrong_size('pos')

        if err_msg: return err_msg

        x, y, z = block_position
        block = (x, y, z, 0,)
        GameStateGlobal.terrainMap.set(*block)
        NetOut.event.set_map([block])

    @logError('set_block')
    @extractPlayer
    def set_block(self, player, **msg):
        err_msg = None
        a = processor.agent_key(player, msg, 'aid')
        if a.error:
            return a.error
        agent = a.agent

        try:
            block_type = int(msg.get('type', None))
        except TypeError:
            err_msg = err.key_missing('type (block)')
        except ValueError:
            err_msg = err.key_invalid('type (block)')

        try:
            block_position = msg['pos']
            block_position = list(block_position)
            assert len(block_position) == 3
        except KeyError:
            err_msg = err.key_missing('pos')
        except ValueError:
            err_msg = err.not_iterable('pos')
        except AssertionError:
            err_msg = err.wrong_size('pos')

        if err_msg: return err_msg
        
        x, y, z = block_position
        block = (x, y, z, block_type,)
        weapon = agent.active_weapon()
        if weapon.fire_command == 'set_block' and weapon.fire():
            print 'setting block'
            GameStateGlobal.terrainMap.set(*block)
            NetOut.event.set_map([block])
            conn.sendMessage.send_weapon(weapon, properties='clip')


class ChatMessageHandler(GenericMessageHandler):

    def events(self):
        return {
            'chat'      :   self.received,
            'subscribe' :   self.subscribe,
            'unsubscribe':  self.unsubscribe,
        }

    def received(self, conn, **msg):
        ChatServer.chat.received(msg, conn)
        
    def subscribe(self, conn, **msg):
        ChatServer.chat.client_subscribe(msg, conn)

    def unsubscribe(self, conn, **msg)
        ChatServer.chat.client_unsubscribe(msg, conn)
