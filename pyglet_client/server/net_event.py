'''
Incoming net events
'''

class NetEvent:

    messageHandler = None
    adminMessageHandler = None

    def __init__(self):
        NetEvent.messageHandler = MessageHandler()
        NetEvent.adminMessageHandler = AdminMessageHandler()

    @classmethod
    def init(cls):
        cls.messageHandler.init()
        cls.adminMessageHandler.init()

from net_server import NetServer
from game_state import GameStateGlobal
from chat_server import ChatServer

# routes messages by msg.cmd
class MessageHandler:

    def init(self):
        pass
    def __init__(self):
        pass

    def process_json(self, msg, connection):
        cmd = msg.get('cmd', None)
        #print "MessageHandler.process_json: " + str(msg)

        # game state
        if cmd == 'create_agent':
            GameStateGlobal.agentList.create(**msg)
        elif cmd == 'agent_control_state':
            self.agent_control_state(connection.id, **msg)
        elif cmd == 'fire_projectile':
            self.fire_projectile(**msg)

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

        agent.set_agent_control_state(tick ,*state)

    def fire_projectile(self, **msg):
        try:
            agent_id = int(msg.get('aid', None))
        except TypeError:
            print 'msg fire_projectile :: agent_id is missing or not an int'
            return

        try:
            agent = GameStateGlobal.agentList[agent_id]
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

# handler for admin msgs
class AdminMessageHandler:

    def init(self):
        pass
    def __init__(self):
        pass

    def process_json(self, msg, connection):
        cmd = msg.get('cmd', None)
        if cmd == "set_map":
            l = msg.get('list', [])
            terrainMap = GameStateGlobal.gameState.terrainMap
            for x,y,z,value in l:
                terrainMap.set(x,y,z,value)
        else:
            print "Admin Message Handler, uncaught message"
