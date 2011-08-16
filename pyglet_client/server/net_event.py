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

    def no_player(client_id):
        return 'could not find player for client %s' % (client_id,)
        
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

    def wrong_size(key, size=None, actual_size=None):
        msg = '%s of wrong size' % (key,)
        if size is not None and actual_size is not None:
            msg = '%s; should be %d, but is %d' % (msg, size, actual_size,)
        return msg

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

class ProcessedIterableMessage:
    __slots__ = ['error', 'iterable']
    def __init__(self, error=None, iterable=None):
        self.error = error
        self.iterable = iterable

class ProcessedItemMessage:
    __slots__ = ['error', 'item']
    def __init__(self, error=None, item=None):
        self.error = error
        self.item = item

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

# for iterable items
    def iterable(msg, key, size, err_key=None):
        if err_key is None:
            err_key = key
        err_msg, obj = None, None

        try:
            obj = msg[key]
            angle = list(obj)
            actual_size = len(obj)
            if actual_size != size:
                err_msg = err.wrong_size(err_key, size, actual_size)
        except KeyError:
            err_msg = err.key_missing(err_key)
        except TypeError:
            err_msg = err.not_iterable(err_key)

        return ProcessedIterableMessage(err_msg, obj)

# for items
    def item(msg, key='iid', err_key=None):
        err_msg, item = None, None
        if err_key is None:
            err_key = key
        try:
            item_id = int(msg.get(key, None))
            item = GameStateGlobal.itemList[item_id]
        except TypeError:
            err_msg = err.key_missing(err_key)
        except ValueError:
            err_msg = err.key_invalid(err_key)
        except KeyError:
            err_msg = err.thing_unknown('item', item_id)

        return ProcessedItemMessage(err_msg, item)

# teams
    def team(msg, key='team', err_key=None):
        if err_key is None:
            err_key = key
        try:
            team_id = int(msg.get(key, None))
            team = GameStateGlobal.teamList[team_id]
        except TypeError:
            err_msg = err.key_missing(err_key)
        except ValueError:
            err_msg = err.key_invalid(err_key)
        except KeyError:
            err_msg = err.thing_unknown('team', team_id)


processor = Processors()


'''
    Decorators
    Use @logError('msg_name') first
'''

# prints full error
def logError(msg_name):
    def outer(f):
        def wrapped(self, conn, *args, **kwargs):
            err = f(self, conn, *args, **kwargs)
            if err is not None:
                print '%s :: %s' % (msg_name, err,)
        return wrapped
    return outer

# retrieves player object based on connection client id
def extractPlayer(f):
    def wrapped(self, conn, *args, **kwargs):
        try:
            player = GameStateGlobal.playerList.client(conn.id)
        except KeyError:
            err_msg = err.no_player(conn.id)
            return err_msg
        return f(self, player, *args, **kwargs)
    return wrapped

# simple require of msg key, use when no extra validation/processing beyond key existance
# the decorated function must include matching arguments positionally,
# in order of decoration, and after any other positional arguments (i.e. connection or player)
def requireKey(key, err_key=None):
    if err_key is None:
        err_key = key
    def outer(f):
        def wrapped(self, *args, **msg):
            try:
                thing = msg[key]
            except KeyError:
                return err.key_missing(err_key)
            args = list(args)
            args.append(thing)
            return f(self, *args, **msg)
        return wrapped

# like requireKey, but also require key to have a certain type
def requireKeyType(key, _type, err_key=None):
    if err_key is None:
        err_key = key
    def outer(f):
        def wrapped(self, *args, **msg):
            try:
                thing = _type(msg.get(key, None))
            except TypeError:
                return err.key_missing(err_key)
            except ValueError:
                return err.key_invalid(err_key)
            args = list(args)
            args.append(thing)
            return f(self, *args, **kwargs)
        return wrapped

# require a msg item to have a certain type, if key is present
def requireTypeIfPresent(key, _type, err_key=None):
    if err_key is None:
        err_key = key
    def outer(f):
        def wrapped(self, *args, **msg):
            thing = None
            try:
                thing = _type(msg[key])
            except ValueError:
                return err.key_invalid(err_key)
            except KeyError:
                pass
            args = list(args)
            args.append(thing)
            return f(self, *args, **kwargs)
        return wrapped
    
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
    @requireKey('id')
    def request_agent(self, connection, aid, **msg):
        connection.sendMessage.send_agent(aid)

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

        p = processor.iterable(msg, 'pos', 9)
        if p.error:
            return p.error
        pos = p.iterable

        agent.state = pos
        NetOut.event.agent_position(agent)

    @logError('agent_button_state')
    @extractPlayer
    def agent_button_state(self, player, **msg):
        a = processor.agent_key(player, msg)
        if a.error:
            return a.error
        agent = a.agent

        p = processor.iterable(msg, 'button', 6)
        if p.error:
            return p.error
        buttons = p.iterable

        if buttons != agent.button_state:
            agent.button_state = buttons
            NetOut.event.agent_button_state(agent)
            ctrl_state = agent.compute_state()
            agent.set_control_state(ctrl_state)

    @logError('agent_angle')
    @extractPlayer
    @requireKey('tick')
    def agent_angle(self, player, tick, **msg):
        err_msg = None
        a = processor.agent_key(player, msg)
        if a.error:
            return a.error
        agent = a.agent

        p = processor.iterable(msg, 'angle', 2)
        if p.error:
            return p.error
        angle = p.iterable

        agent.set_angle(angle)


class PlayerMessageHandler:

    def events(self):
        return {
            'request_player'    :   self.request_player,
        }

    @logError('request_player')
    @requireKey('id')
    def request_player(self, connection, pid, **msg):
        connection.sendMessage.send_player(pid)

class ProjectileMessageHandler:

    def events(self):
        return {
            'request_projectile':   self.request_projectile,
            'fire_projectile'   :   self.fire_projectile,
            'throw_grenade' : self.throw_grenade,
        }

    @logError('request_projectile')
    @requireKey('id')
    def request_projectile(self, conn, prid, **msg):
        conn.sendMessage.send_projectile(prid)

    @logError('throw_grenade')
    @extractPlayer
    def throw_grenade(self, player, **msg):
        a = processor.agent_key(player, msg, 'aid')
        if a.error:
            return a.error
        agent = a.agent

        p = processor.iterable(msg, 'vector', 3)
        if p.error:
            return p.error
        vector = p.iterable

        weapon = agent.active_weapon()
        if weapon.fire_command == 'throw_grenade' and weapon.fire():
            agent.throw_grenade(vector)

    @logError('fire_projectile')
    @extractPlayer
    def fire_projectile(self, player, **msg):
        a = processor.agent_key(player, msg, 'aid')
        if a.error:
            return a.error
        agent = a.agent

        p = processor.iterable(msg, 'pos', 3)
        if p.error:
            return p.error
        pos = p.iterable

        p = processor.iterable(msg, 'vec', 3, 'vec (direction)')
        if p.error:
            return p.error
        vec = p.iterable

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
    @requireKey('id')
    def request_weapon(self, conn, wid, **msg):
        conn.sendMessage.send_weapon(wid)

    @logError('reload_weapon')
    @extractPlayer
    @requireKeyType('weapon', int)
    def reload_weapon(self, player, weapon_type, **msg):
        a = processor.agent_key(player, msg, 'aid')
        if a.error:
            return a.error
        agent = a.agent
            
        try:
            weapon_index = [weapon.type for weapon in agent.weapons].index(weapon_type)
        except ValueError:
            return 'weapon unknown to agent'

        weapon = agent.weapons[weapon_index]
        if weapon.reload():
            NetOut.event.agent_update(agent, 'weapons')

    @logError('hitscan')
    @extractPlayer
    @requireKey('target')
    def hitscan(self, player, target, **msg):
        err_msg = None
        
        a = processor.agent(player, msg)
        if a.error:
            return a.error
        firing_agent = a.agent
            
        weapon = firing_agent.active_weapon()
        if not weapon.hitscan:
            return 'Client sent hitscan message for non-hitscan weapon'

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
    @requireKey('windex', err_key='windex (active_weapon)')
    def change_weapon(self, player, active_weapon, **msg):
        a = processor.agent_key(player, msg, 'aid')
        if a.error:
            return a.error
        agent = a.agent

        if active_weapon == -1: # json doesnt have None, but None is a valid input; careful, -1 is a valid index.
            active_weapon = None

        agent.set_active_weapon(active_weapon)

    @logError('drop_weapon')
    @extractPlayer
    @requireKey('wid')
    def drop_weapon(self, player, weapon_id, **msg):
        a = processor.agent_key(player, msg, 'aid')
        if a.error:
            return a.error
        agent = a.agent

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
    @requireKey('id')
    def request_item(self, conn, iid, **msg):
        conn.sendMessage.send_item(iid)

    @logError('pickup_item')
    @extractPlayer
    @requireTypeIfPresent('slot', int)
    def pickup_item(self, player, slot, **msg):
        a = processor.agent_key(player, msg, 'aid')
        if a.error:
            return a.error
        agent = a.agent

        i = processor.item(msg)
        if i.error:
            return i.error
        item = i.item
        
        if agent.near_item(item):
            print 'agent picking up %s' % (item,)
            agent.pickup_item(item, slot)

    @logError('near_item')
    @extractPlayer
    def near_item(self, player, **msg):
        a = processor.agent_key(player, msg, 'aid')
        if a.error:
            return a.error
        agent = a.agent

        i = processor.item(msg)
        if i.error:
            return i.error
        item = i.item

        if agent.near_item(item) and hasattr(item, 'agent_nearby'):
            item.agent_nearby(agent)

    @logError('drop_item')
    @extractPlayer
    def drop_item(self, player, **msg):
        a = processor.agent_key(player, msg, 'aid')
        if a.error:
            return a.error
        agent = a.agent

        i = processor.item(msg)
        if i.error:
            return i.error
        item = i.item

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
        t = processor.team(msg)
        if t.error:
            return t.error
        team = t.team
            
        GameStateGlobal.game.player_join_team(player, team)

    @logError('request_team')
    @requireKey('id')
    def request_team(self, conn, tid, **msg):
        conn.sendMessage.send_team(tid)


class MiscMessageHandler:

    def events(self):
        return {
            'ping'  :   self.ping,
            'identify': self.identify,
            'request_client_id':    self.request_client_id,
        }

    @logError('ping')
    @requireKey('timestamp')
    def ping(self, connection, ts, **msg):
        connection.sendMessage.ping(ts)

    @logError('identify')
    @requireKey('name')
    def identify(self, conn, name, **msg):
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
        p = processor.iterable(msg, 'value', 3)
        if p.error:
            return p.error
        x,y,z = p.iterable

        connection.sendMessage.send_chunk(x,y,z)

    @logError('send_chunk_list')
    def send_chunk_list(self, connection, **msg):
        connection.sendMessage.send_chunk_list()

    @logError('hit_block')
    @extractPlayer
    def hit_block(self, player, **msg):
        a = processor.agent_key(player, msg, 'aid')
        if a.error:
            return a.error
        agent = a.agent

        p = processor.iterable('pos', 3)
        if p.error:
            return p.error
        x,y,z = p.iterable

        block = (x, y, z, 0,)
        GameStateGlobal.terrainMap.set(*block)
        NetOut.event.set_map([block])

    @logError('set_block')
    @extractPlayer
    @requireKeyType('type', int, err_key='type (block)')
    def set_block(self, player, block_type, **msg):
        a = processor.agent_key(player, msg, 'aid')
        if a.error:
            return a.error
        agent = a.agent

        p = processor.iterable('pos', 3)
        if p.error:
            return p.error
        x,y,z = p.iterable

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
