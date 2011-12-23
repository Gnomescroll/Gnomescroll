'''
Incoming net events
'''

import c_lib.c_lib_game_modes as cGame

class NetEvent:

    messageHandler = None
    adminMessageHandler = None
    miscMessageHandler = None
    agentMessageHandler = None
    mapMessageHandler = None
    playerMessageHandler = None
    weaponMessageHandler = None
    gameModeMessageHandler = None
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
        cls.gameModeMessageHandler = GameModeMessageHandler()
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

    def _default_thing(self, thing_name, processed_msg_obj, msg, key, err_key=None, datastore=None, SKIP_DATASTORE=False):
        if datastore is None:
            datastore = '%sList' % (thing_name,)
        err_msg, thing = None, None
        if err_key is None:
            err_key = key

        try:
            thing = thing_id = int(msg.get(key, None))
            if not SKIP_DATASTORE:
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
        m = self._default_thing('team', ProcessedTeamMessage, msg, key, err_key, SKIP_DATASTORE=True)
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
            'admin'     : self.is_admin,
            'set_map' : self._set_map,
            'clear_map': self._clear_map,
            'save_map': self._save_map,
        }

    def is_admin(self, msg, conn):
        conn.admin = True

    @requireKey('list')
    def _set_map(self, msg, conn, blocks):
        terrainMap = GameStateGlobal.terrainMap
        for x,y,z,value in blocks:
            terrainMap.set(x,y,z,value)
        NetOut.event.set_map(blocks)

    def _clear_map(self, msg, conn):
        GameStateGlobal.terrainMap.clear()
        NetOut.event.clear_map()

    @requireKey('name')
    def _save_map(self, msg, conn, name):
        GameStateGlobal.terrainMap.save_to_disk(name)

class AgentMessageHandler(GenericMessageHandler):

    def events(self):
        return {
            'request_agent'     :   self.request_agent,
        }

    @logError('request_agent')
    @requireKey('id')
    def request_agent(self, msg, connection, aid):
        connection.sendMessage.send_agent(aid)

class PlayerMessageHandler(GenericMessageHandler):

    def events(self):
        return {
            'request_player'    :   self.request_player,
        }

    @logError('request_player')
    @requireKey('id')
    def request_player(self, msg, connection, pid):
        connection.sendMessage.send_player(pid)

'''
move reload, to C
Deprecate drop weapon
'''
class WeaponMessageHandler(GenericMessageHandler):

    def events(self):
        return {
            'request_weapon'    :   self.request_weapon, #deprecate
            'reload_weapon'     :   self.reload_weapon, #move to c
            'drop_weapon'       :   self.drop_weapon, #deprecate
            'change_weapon'     :   self.change_weapon, #move to C
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


class GameModeMessageHandler(GenericMessageHandler):

    def events(self):
        return {
            'join_team' :   self.join_team,
        }

    @logError('join_team')
    @extractPlayer
    @processTeam()
    def join_team(self, msg, player, team):
        if player.agent is not None:
            cGame.join_team(player.agent.id, team)

class MiscMessageHandler(GenericMessageHandler):

    def events(self):
        return {
            'ping'  :   self.ping,
            'identify': self.identify,
        }

    @logError('ping')
    @requireKey('timestamp')
    def ping(self, msg, connection, ts):
        connection.sendMessage.ping(ts)

    @logError('identify')
    @requireKey('name')
    def identify(self, msg, conn, name):
        conn.identify(name)


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
        dmg = GameStateGlobal.terrainMap.apply_damage(x, y, z, w.damage)
        if not dmg: # dmg=0 if block was destroyed
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
