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
    def init(self):
        self.messageHandler.init()
        self.adminMessageHandler.init()

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
        if cmd == 'create_agent':
            GameStateGlobal.gameState.create_agent(**msg)
        elif cmd == 'agent_control_state':
            self.agent_control_state(msg)
        elif cmd == 'chat':
            ChatServer.chat.received(msg, connection)
        elif cmd == 'subscribe':
            ChatServer.chat.client_subscribe(msg, connection)
        elif cmd == 'unsubscribe':
            ChatServer.chat.client_unsubscribe(msg, connection)
        elif cmd == 'send_client_id': #Setup client connection
            connection.set_client_id(int(msg['id']))
            #print "Client Assigned id= %i" % (connection.client_id,)
        elif cmd == 'request_chunk_list':
            self.send_chunk_list(msg, connection)
        elif cmd == 'request_chunk':
            print "chunk_request"
            self.request_chunk(msg, connection)
        else:
            print "MessageHandler.process_json: cmd unknown = %s" % (str(msg),)

    def agent_control_state(self, msg):
        try:
            id = int(msg.get('id', None))
        except TypeError:
            print 'msg.cmd == agent_control_state, but msg.id missing. MSG: %s' % (str(msg),)
            return
        except ValueError:
            print 'msg.cmd == agent_control_state, but msg.id is not an int. MSG: %s' % (str(msg),)
            return
        try:
            agent = GameStateGlobal.agentList[id]
        except KeyError:
            print 'msg.cmd == agent_control_state, msg.id is not a known agent'
            return
        tick = msg.get('tick', None)
        if tick is None:
            print 'msg agent_control_state missing "tick"'
            return
        state = msg.get('state', [None for i in range(6)])
        if state is None:
            print 'msg agent_control_state missing "state"'
            return
        state = list(state)
        state.append(tick)
        agent.set_agent_control_state(*state)

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
