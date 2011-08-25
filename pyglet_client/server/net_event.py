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
    datMessageHandler = None
    
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
        cls.datMessageHandler = DatMessageHandler()
        
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
            self.json_events[cmd](msg, connection)
        else:
            print "MessageHandler.process_json: cmd unknown = %s" % (str(msg),)

'''
Common Errors
'''

class ErrorNames:

    def no_player(self, client_id):
        return 'could not find player for client %s' % (client_id,)
        
    def agent_ownership(self, p, a):
        return 'player %d does not own agent %d' % (p.id, a.id,)

    def is_viewer(self, msg):
        return 'ignoring viewing agent; cmd %s' % (msg['cmd'],)

    def key_missing(self, key):
        return '%s missing' % (key,)

    def key_invalid(self, key):
        return '%s invalid' % (key,)

    def key_invalid_or_missing(self, key):
        return '%s invalid or missing' % (key,)

    def wrong_size(self, key, size=None, actual_size=None):
        msg = '%s of wrong size' % (key,)
        if size is not None and actual_size is not None:
            msg = '%s; should be %d, but is %d' % (msg, size, actual_size,)
        return msg

    def not_iterable(self, key):
        return '%s is not iterable' % (key,)

    def thing_unknown(self, name, thing_id):
        return '%s %d unknown' % (name, thing_id,)

    def player_has_no_agent(self, player):
        return 'player %d has no agent' % (player.id,)

err = ErrorNames()

        
'''
Processors, for common message items/keys
'''

class ProcessedAgentMessage:
    __slots__ = ['error', 'agent']
    def __init__(self, error, agent):
        self.error = error
        self.agent = agent

class ProcessedIterableMessage:
    __slots__ = ['error', 'iterable']
    def __init__(self, error, iterable):
        self.error = error
        self.iterable = iterable

class ProcessedItemMessage:
    __slots__ = ['error', 'item']
    def __init__(self, error, item):
        self.error = error
        self.item = item

class ProcessedTeamMessage:
    __slots__ = ['error', 'team']
    def __init__(self, error, team):
        self.error = error
        self.team = team

class Processors:

# for msgs that assume agent is player's agent
    def player_agent(self, player, msg):
        err_msg, agent = None, None
        try:
            agent = player.agent
            if agent.team.is_viewers():
                err_msg = err.is_viewer(msg)
        except AttributeError:
            err_msg = err.player_has_no_agent(player)
        return ProcessedAgentMessage(err_msg, agent)

    def _default_thing(self, thing_name, processed_msg_obj, msg, key, err_key=None, datastore=None):
        if datastore is None:
            datastore = '%sList' % (thing_name,)
        err_msg, thing = None, None
        if err_key is None:
            err_key = key

        try:
            thing_id = int(msg.get(key, None))
            thing = getattr(GameStateGlobal, datastore)[thing_id]
        except TypeError:
            err_msg = err.key_missing(err_key)
        except ValueError:
            err_msg = err.key_invalid(err_key)
        except KeyError:
            err_msg = err.thing_unknown(thing_name, thing_id)

        return processed_msg_obj(err_msg, thing)

# for msgs that send agent id and want it extracted and checked against player
    def agent(self, player, msg, key='id', err_key=None):
        m = self._default_thing('agent', ProcessedAgentMessage, msg, key, err_key)
        if m.error:
            return m

        if m.agent.team.is_viewers():
            m.error = err.is_viewer(msg)
        if not player.owns(m.agent):
            m.error = err.agent_ownership(player, m.agent)

        return m

    def item(self, msg, key='iid', err_key=None):
        m = self._default_thing('item', ProcessedItemMessage, msg, key, err_key)
        return m

    def team(self, msg, key='team', err_key=None):
        m = self._default_thing('team', ProcessedTeamMessage, msg, key, err_key)
        return m

# for iterable items
    def iterable(self, msg, key, size, err_key=None):
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

processor = Processors()


'''
    Decorators
    Use @logError('msg_name') first

    The other decorators insert arguments, so make sure to align the decorated
    function's arguments with the order in which the decorators were applied
'''

# THIS IS NOT A DECORATOR, it is just used by decorators
def _add_arg(args, item):
    args = list(args)
    args.append(item)
    return args

# prints full error
def logError(msg_name):
    def outer(f):
        def wrapped(self, msg, conn, *args, **kwargs):
            err = f(self, msg, conn, *args, **kwargs)
            if err is not None:
                print '%s :: %s' % (msg_name, err,)
        return wrapped
    return outer

# retrieves player object based on connection client id
def extractPlayer(f):
    def wrapped(self, msg, conn, *args, **kwargs):
        try:
            player = GameStateGlobal.playerList.client(conn.id)
        except KeyError:
            err_msg = err.no_player(conn.id)
            return err_msg
        args = _add_arg(args, player)
        return f(self, msg, *args, **kwargs)
    return wrapped

# simple require of msg key, use when no extra validation/processing beyond key existance
# the decorated function must include matching arguments positionally,
# in order of decoration, and after any other positional arguments (i.e. connection or player)
def requireKey(key, err_key=None):
    if err_key is None:
        err_key = key
    def outer(f, *args, **kwargs):
        def wrapped(self, msg, *args, **kwargs):
            try:
                thing = msg[key]
            except KeyError:
                return err.key_missing(err_key)
            args = _add_arg(args, thing)
            return f(self, msg, *args, **kwargs)
        return wrapped
    return outer

# like requireKey, but also require key to have a certain type
def requireKeyType(key, _type, err_key=None):
    if err_key is None:
        err_key = key
    def outer(f, *args, **kwargs):
        def wrapped(self, msg, *args, **kwargs):
            try:
                thing = _type(msg.get(key, None))
            except TypeError:
                return err.key_missing(err_key)
            except ValueError:
                return err.key_invalid(err_key)
            args = _add_arg(args, thing)
            return f(self, msg, *args, **kwargs)
        return wrapped
    return outer

# require a msg item to have a certain type, if key is present
def requireTypeIfPresent(key, _type, err_key=None):
    if err_key is None:
        err_key = key
    def outer(f, *args, **kwargs):
        def wrapped(self, msg, *args, **kwargs):
            thing = None
            try:
                thing = _type(msg[key])
            except ValueError:
                return err.key_invalid(err_key)
            except KeyError:
                pass
            args = _add_arg(args, thing)
            return f(self, msg, *args, **kwargs)
        return wrapped
    return outer

def processAgent(key='id', err_key=None):
    def outer(f, *args, **kwargs):
        def wrapped(self, msg, player, *args, **kwargs):
            a = processor.agent(player, msg, key, err_key)
            if a.error:
                return a.error
            args = _add_arg(args, a.agent)
            return f(self, msg, player, *args, **kwargs)
        return wrapped
    return outer

def processPlayerAgent(f, *args, **kwargs):
    def wrapped(self, msg, player, *args, **kwargs):
        a = processor.player_agent(player, msg)
        if a.error:
            return a.error
        args = _add_arg(args, a.agent)
        return f(self, msg, player, *args, **kwargs)
    return wrapped

def processIterable(key, size, err_key=None):
    def outer(f, *args, **kwargs):
        def wrapped(self, msg, *args, **kwargs):
            p = processor.iterable(msg, key, size, err_key)
            if p.error:
                return p.error
            args = _add_arg(args, p.iterable)
            return f(self, msg, *args, **kwargs)
        return wrapped
    return outer

def processItem(key='iid', err_key=None):
    def outer(f, *args, **kwargs):
        def wrapped(self, msg, *args, **kwargs):
            a = processor.item(msg, key, err_key)
            if a.error:
                return a.error
            args = _add_arg(args, a.item)
            return f(self, msg, *args, **kwargs)
        return wrapped
    return outer

def processTeam(key='team', err_key=None):
    def outer(f, *args, **kwargs):
        def wrapped(self, msg, *args, **kwargs):
            a = processor.team(msg, key, err_key)
            if a.error:
                return a.error
            args = _add_arg(args, a.team)
            return f(self, msg, *args, **kwargs)
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

        
class AdminMessageHandler(GenericMessageHandler):

    def events(self):
        return {
            'set_map' : self._set_map,
        }

    def _set_map(self, list, msg):
        terrainMap = GameStateGlobal.terrainMap
        for x,y,z,value in list:
            terrainMap.set(x,y,z,value)
        NetOut.event.set_map(list)


class AgentMessageHandler(GenericMessageHandler):

    def events(self):
        return {
            'request_agent'     :   self.request_agent,
            'agent_position'    :   self.agent_position,
            'agent_button_state':   self.agent_button_state,
            'agent_angle'       :   self.agent_angle,
            'create_agent'      :   self.create_agent,
        }

    @logError('request_agent')
    @requireKey('id')
    def request_agent(self, msg, connection, aid):
        connection.sendMessage.send_agent(aid)

    @logError('create_agent')
    def create_agent(self, msg, connection):
        print 'Received message CREATE_AGENT. Why????'
        GameStateGlobal.agentList.create(msg)

    @logError('agent_position')
    @extractPlayer
    @processAgent()
    @processIterable('pos', 9)
    def agent_position(self, msg, player, agent, pos):
        agent.state = pos
        NetOut.event.agent_position(agent)

    @logError('agent_button_state')
    @extractPlayer
    @processAgent()
    @processIterable('buttons', 6)
    def agent_button_state(self, msg, player, agent, buttons):
        if buttons != agent.button_state:
            agent.button_state = buttons
            NetOut.event.agent_button_state(agent)
            ctrl_state = agent.compute_state()
            agent.set_control_state(ctrl_state)

    @logError('agent_angle')
    @extractPlayer
    @processAgent()
    @processIterable('angle', 2)
    @requireKey('tick')
    def agent_angle(self, msg, player, agent, angle, tick):
        agent.set_angle(angle)


class PlayerMessageHandler(GenericMessageHandler):

    def events(self):
        return {
            'request_player'    :   self.request_player,
        }

    @logError('request_player')
    @requireKey('id')
    def request_player(self, msg, connection, pid):
        connection.sendMessage.send_player(pid)
        

class ProjectileMessageHandler(GenericMessageHandler):

    def events(self):
        return {
            'request_projectile':   self.request_projectile,
            'fire_projectile'   :   self.fire_projectile,
            'throw_grenade' : self.throw_grenade,
        }

    @logError('request_projectile')
    @requireKey('id')
    def request_projectile(self, msg, conn, prid):
        conn.sendMessage.send_projectile(prid)

    @logError('throw_grenade')
    @extractPlayer
    @processAgent('aid')
    @processIterable('vector', 3)
    def throw_grenade(self, msg, player, agent, vector):
        weapon = agent.active_weapon()
        if weapon.fire_command == 'throw_grenade' and weapon.fire():
            agent.throw_grenade(vector)

    @logError('throw_grenade_c')
    @extractPlayer
    @processAgent('aid')
    @processIterable('dir', 3)
    @processIterable('vel', 3)
    def throw_grenade_c(self, msg, player, agent, direction, velocity):
        weapon = agent.active_weapon()
        if weapon.fire_command == 'throw_grenade_c' and weapon.fire():
            agent.throw_grenade_c(direction, velocity)

    @logError('fire_projectile')
    @extractPlayer
    @processAgent('aid')
    @processIterable('pos', 3)
    @processIterable('vec', 3, err_key='vec (direction)')
    def fire_projectile(self, msg, player, agent, pos, vec):
        weapon = agent.active_weapon()
        if weapon.fire_command == 'fire_projectile' and weapon.fire():
            agent.fire_projectile(pos=pos, direction=vec)

    
class WeaponMessageHandler(GenericMessageHandler):

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
    def request_weapon(self, msg, conn, wid):
        conn.sendMessage.send_weapon(wid)

    @logError('reload_weapon')
    @extractPlayer
    @processAgent('aid')
    @requireKeyType('weapon', int)
    def reload_weapon(self, msg, player, agent, weapon_type):
        try:
            weapon_index = [weapon.type for weapon in agent.weapons].index(weapon_type)
        except ValueError:
            return 'weapon unknown to agent'

        weapon = agent.weapons[weapon_index]
        if weapon.reload():
            NetOut.event.agent_update(agent, 'weapons')

    @logError('hitscan')
    @extractPlayer
    @processPlayerAgent
    @requireKey('target')
    def hitscan(self, msg, player, firing_agent, target):
        err_msg = None
        
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
    @processAgent('aid')
    @requireKey('windex', err_key='windex (active_weapon)')
    def change_weapon(self, msg, player, agent, active_weapon):
        if active_weapon == -1: # json doesnt have None, but None is a valid input; careful, -1 is a valid index.
            active_weapon = None
        agent.set_active_weapon(active_weapon)

    @logError('drop_weapon')
    @extractPlayer
    @processAgent('aid')
    @requireKey('wid')
    def drop_weapon(self, msg, player, agent, weapon_id):
        agent.drop_weapon(weapon_id, by_id=True)


class ItemMessageHandler(GenericMessageHandler):

    def events(self):
        return {
            'request_item'  :   self.request_item,
            'near_item'     :   self.near_item,
            'pickup_item'   :   self.pickup_item,
            'drop_item'     :   self.drop_item,
        }

    @logError('request_item')
    @requireKey('id')
    def request_item(self, msg, conn, iid):
        conn.sendMessage.send_item(iid)

    @logError('pickup_item')
    @extractPlayer
    @processAgent('aid')
    @processItem()
    @requireTypeIfPresent('slot', int)
    def pickup_item(self, msg, player, agent, item, slot):
        if agent.near_item(item):
            print 'agent picking up %s' % (item,)
            agent.pickup_item(item, slot)

    @logError('near_item')
    @extractPlayer
    @processAgent('aid')
    @processItem()
    def near_item(self, msg, player, agent, item):
        if agent.near_item(item) and hasattr(item, 'agent_nearby'):
            item.agent_nearby(agent)

    @logError('drop_item')
    @extractPlayer
    @processAgent('aid')
    @processItem()
    def drop_item(self, msg, player):
        agent.drop_item(item)


class GameModeMessageHandler(GenericMessageHandler):

    def events(self):
        return {
            'join_team' :   self.join_team,
            'request_team': self.request_team,
        }

    @logError('join_team')
    @extractPlayer
    @processTeam()
    def join_team(self, msg, player, team):
        GameStateGlobal.game.player_join_team(player, team)

    @logError('request_team')
    @requireKey('id')
    def request_team(self, msg, conn, tid):
        conn.sendMessage.send_team(tid)


class MiscMessageHandler(GenericMessageHandler):

    def events(self):
        return {
            'ping'  :   self.ping,
            'identify': self.identify,
            'request_client_id':    self.request_client_id,
            'received_client_id':   self.received_client_id,
        }

    @logError('ping')
    @requireKey('timestamp')
    def ping(self, msg, connection, ts):
        connection.sendMessage.ping(ts)

    @logError('identify')
    @requireKey('name')
    def identify(self, msg, conn, name):
        conn.identify(name)

    def request_client_id(self, msg, conn):
        conn.send_client_id()

    def received_client_id(self, msg, conn):
        conn.set_id_received()

class MapMessageHandler(GenericMessageHandler):

    def events(self):
        return {
            'request_chunk' :   self.request_chunk,
            'request_chunk_list':  self.send_chunk_list,
            'hit_block'     :   self.hit_block,
            'set_block'     :   self.set_block,
        }

    @logError('request_chunk')
    @processIterable('value', 3)
    def request_chunk(self, msg, connection, block):
        x,y,z = block
        connection.sendMessage.send_chunk(x,y,z)

    @logError('send_chunk_list')
    def send_chunk_list(self, msg, connection):
        connection.sendMessage.send_chunk_list()

    @logError('hit_block')
    @extractPlayer
    @processAgent('aid')
    @processIterable('pos', 3)
    def hit_block(self, msg, player, agent, pos):
        w = agent.active_weapon()
        if not (w.fire_command == 'hit_block' and w.fire()): # pick
            return
        x,y,z = pos
        died = GameStateGlobal.terrainMap.apply_damage(x, y, z, w.damage)
        if died:
            NetOut.event.set_map([(x,y,z,0)])

    @logError('set_block')
    @extractPlayer
    @processAgent('aid')
    @processIterable('pos', 3)
    @requireKeyType('type', int, err_key='type (block)')
    def set_block(self, msg, player, agent, pos, block_type):
        x,y,z = pos
        block = (x, y, z, block_type,)
        weapon = agent.active_weapon()
        if weapon.fire_command == 'set_block' and weapon.fire():
            print 'setting block'
            GameStateGlobal.terrainMap.set(*block)
            NetOut.event.set_map([block])
            player.connection.sendMessage.send_weapon(weapon, properties='clip')


class ChatMessageHandler(GenericMessageHandler):

    def events(self):
        return {
            'chat'      :   self.received,
            'subscribe' :   self.subscribe,
            'unsubscribe':  self.unsubscribe,
        }

    def received(self, msg, conn):
        ChatServer.chat.received(msg, conn)
        
    def subscribe(self, msg, conn):
        ChatServer.chat.client_subscribe(msg, conn)

    def unsubscribe(self, msg, conn):
        ChatServer.chat.client_unsubscribe(msg, conn)


class DatMessageHandler(GenericMessageHandler):

    def events(self):
        return {
            'dat_loaded'    :   self.loaded,
        }

    def loaded(self, msg, conn):
        conn.set_dat_loaded()
