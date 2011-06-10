'''
Incoming net events
'''

class NetEvent:

    messageHandler = None
    adminMessageHandler = None

    @classmethod
    def init_0(cls):
        cls.messageHandler = MessageHandler()
        cls.adminMessageHandler = AdminMessageHandler()

    @classmethod
    def init_1(cls):
        cls.messageHandler.init()
        cls.adminMessageHandler.init()

    @classmethod
    def register_json_events(cls, events):
        for string, function in events.items():
            cls.messageHandler.json_events[string] = function

from net_server import NetServer
from net_out import NetOut
from game_state import GameStateGlobal
from chat_server import ChatServer


class AgentMessageHandler:
    pass

class PlayerMessageHandler:
    pass

# routes messages by msg.cmd
class MessageHandler:

    def init(self):
        pass
    def __init__(self):
        self.json_events = {} #map strings to functions

    def process_json(self, msg, connection):
        cmd = msg.get('cmd', None)
        #print "MessageHandler.process_json: " + str(msg)

        #use json_events when possible
        if self.json_events.has_key(cmd):
            self.json_events[cmd](**msg)
        # game state
        elif cmd == 'create_agent':
            GameStateGlobal.agentList.create(**msg)
        elif cmd == 'agent_control_state':
            self.agent_control_state(connection.id, **msg)
        elif cmd == 'request_agent':
            self.request_agent(connection, **msg)

        elif cmd == 'fire_projectile':
            self.fire_projectile(connection.id, **msg)
        elif cmd == 'reload_weapon':
            self.reload_weapon(connection.id, **msg)

        elif cmd == 'set_block':
            self.set_block(connection.id, **msg)
        elif cmd == 'hit_block':
            self.hit_block(connection.id, **msg)


        #chat
        elif cmd == 'chat':
            ChatServer.chat.received(msg, connection)
        elif cmd == 'subscribe':
            ChatServer.chat.client_subscribe(msg, connection)
        elif cmd == 'unsubscribe':
            ChatServer.chat.client_unsubscribe(msg, connection)

        # setup
        elif cmd == 'identify': #Setup client connection
            name = msg.get('name', None)
            if name is None:
                print 'msg identify - name is missing'
                return
            connection.identify(name)
        elif cmd == 'request_client_id':
            connection.send_client_id()

        # map
        elif cmd == 'request_chunk_list':
            self.send_chunk_list(msg, connection)
        elif cmd == 'request_chunk':
            self.request_chunk(msg, connection)
        else:
            print "MessageHandler.process_json: cmd unknown = %s" % (str(msg),)

    def set_block(self, client_id, **msg):
        try:
            player = GameStateGlobal.playerList.client(client_id)
        except KeyError:
            print 'msg set_block :: Could not find player for client'
            return
        try:
            agent_id = int(msg.get('aid', None))
            agent = GameStateGlobal.agentList[agent_id]
            if agent.owner != player.id:
                print 'msg set_block :: client does not own this agent'
                return
        except TypeError:
            print 'msg set_block :: aid missing'
            return
        except ValueError:
            print 'msg set_block :: aid invalid'
            return
        except KeyError:
            print 'msg set_block :: agent unknown'
            return

        try:
            block_type = int(msg.get('type', None))
        except TypeError:
            print 'msg set_block :: type missing'
            return
        except ValueError:
            print 'msg set_block :: type invalid'
            return

        try:
            block_position = msg['pos']
            block_position = list(block_position)
            assert len(block_position) == 3
        except KeyError:
            print 'msg set_block :: pos missing'
            return
        except ValueError:
            print 'msg set_block :: pos not iterable'
            return
        except AssertionError:
            print 'msg set_block :: block pos length is not 3'
            return
        x, y, z = block_position
        block = (x, y, z, block_type,)
        GameStateGlobal.terrainMap.set(*block)
        NetOut.event.set_map([block])


    def hit_block(self, client_id, **msg):
        try:
            player = GameStateGlobal.playerList.client(client_id)
        except KeyError:
            print 'msg hit_block :: Could not find player for client'
            return
        try:
            agent_id = int(msg.get('aid', None))
            agent = GameStateGlobal.agentList[agent_id]
            if agent.owner != player.id:
                print 'msg hit_block :: client does not own this agent'
                return
        except TypeError:
            print 'msg hit_block :: aid missing'
            return
        except ValueError:
            print 'msg hit_block :: aid invalid'
            return
        except KeyError:
            print 'msg hit_block :: agent unknown'
            return

        try:
            block_position = msg['pos']
            block_position = list(block_position)
            assert len(block_position) == 3
        except KeyError:
            print 'msg hit_block :: pos missing'
            return
        except ValueError:
            print 'msg hit_block :: pos not iterable'
            return
        except AssertionError:
            print 'msg hit_block :: block pos length is not 3'
            return
        x, y, z = block_position
        block = (x, y, z, 0,)
        print 'hit_block', block
        GameStateGlobal.terrainMap.set(*block)
        NetOut.event.set_map([block])

    def reload_weapon(self, client_id, **msg):
        try:
            player = GameStateGlobal.playerList.client(client_id)
        except KeyError:
            print 'msg reload_weapon :: Could not find player for client'
            return
        try:
            agent_id = int(msg.get('aid', None))
            agent = GameStateGlobal.agentList[agent_id]
            if agent.owner != player.id:
                print 'msg reload_weapon :: client does not own this agent'
                return
        except TypeError:
            print 'msg reload_weapon :: aid missing'
            return
        except ValueError:
            print 'msg reload_weapon :: aid invalid'
            return
        except KeyError:
            print 'msg reload_weapon :: agent unknown'
            return

        try:
            weapon_type = int(msg.get('weapon', None))
        except TypeError:
            print 'msg reload_weapon :: weapon missing'
            return
        except ValueError:
            print 'msg reload_weapon :: weapon invalid'
            return
        try:
            weapon_index = [weapon.type for weapon in agent.weapons].index(weapon_type)
        except ValueError:
            print 'msg reload_weapon :: weapon unknown to agent'
            return
        weapon = agent.weapons[weapon_index]
        if weapon.reload():
            NetOut.event.agent_update(agent, 'weapons')

    def agent_control_state(self, client_id, **msg):
        try:
            agent = GameStateGlobal.playerList.client(client_id).agent
        except KeyError:
            print 'msg.cmd == agent_control_state, msg.id is not a known client'
            return
        except AttributeError:
            print 'msg.cmd == agent_control_state, player has no agent'
            return

        tick = msg.get('tick', None)
        if tick is None:
            print 'msg agent_control_state missing "tick"'
            return

        try:
            state = msg['state']
            state = list(state)
            assert len(state) == 7
        except KeyError:
            print 'msg agent_control_state missing "state"'
            return
        except TypeError:
            print 'msg agent_control_state :: state is not iterable'
            return
        except AssertionError:
            print 'msg agent_control_state :: state has wrong number of elements'
            return

        try:
            angle = msg['angle']
            angle = list(angle)
            assert len(angle) == 2
        except KeyError:
            print 'msg agent_control_state :: missing "angle"'
            return
        except TypeError:
            print 'msg agent_control_state :: angle is not iterable'
            return
        except AssertionError:
            print 'msg agent_control_state :: angle has wrong number of elements'
            return

        agent.set_agent_control_state(tick, state, angle)

    def fire_projectile(self, client_id, **msg):
        try:
            player = GameStateGlobal.playerList.client(client_id)
        except KeyError:
            print 'msg fire_projectile :: Could not find player for client'
            return
        try:
            agent_id = int(msg.get('aid', None))
            agent = GameStateGlobal.agentList[agent_id]
            if agent.owner != player.id:
                print 'msg fire_projectile :: player does not own this agent'
                return
        except TypeError:
            print 'msg fire_projectile :: agent_id is missing or not an int'
            return
        except KeyError:
            print 'msg fire_projectile :: agent %i unknown' % (agent_id,)
            return
        agent.fire_projectile()

    def send_chunk_list(self, msg, connection):
        connection.sendMessage.send_chunk_list()

    def request_chunk(self, msg, connection):
        try:
            x,y,z = msg['value']
        except KeyError:
            print 'msg request_chunk, "value" missing'
            return
        except ValueError:
            print 'msg request_chunk, "value" must be a 3 tuple'
            return
        connection.sendMessage.send_chunk(x,y,z)

    def request_agent(self, connection, **msg):
        if 'aid' not in msg:
            return
        agent_id = msg['aid']
        connection.sendMessage.send_agent(agent_id)


# handler for admin msgs
class AdminMessageHandler:

    def init(self):
        pass
    def __init__(self):
        self.register_events()
    def register_events(self):
        events = {
            'set_map' : self._set_map,
        }
        NetEvent.register_json_events(events)

    def _set_map(self, list, **msg):
        terrainMap = GameStateGlobal.terrainMap
        for x,y,z,value in list:
            terrainMap.set(x,y,z,value)
        NetOut.event.set_map(list)
