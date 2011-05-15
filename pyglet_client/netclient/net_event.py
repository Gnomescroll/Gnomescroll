
import simple_json as json
import struct

class NetEventGlobal:
    sendMessage = None
    messageHandler = None

    def __init__(self):
        netEventGlobal.sendMessage = SendMessage()
        netEventGlobal.messageHandler = messageHandler()
    @classmethod
    def init(self):
        netEventGlobal.sendMessage.init()
        netEventGlobal.messageHandler.init()

def add_prefix(self, id, msg):
    return struct.pack('I H', 4+2+len(msg), id) + msg #length prefix is included in length
def send_json(self, dict):
    self.client.send(self.add_prefix(1, json.dumps(dict)))  #fix this
def get_json(self, dict):
    return add_prefix(1, json.dumps(dict))

class SendMessage:

    @classmethod
    def init(self):
        pass
    def __init__(self):
        self.client = client

    ### agent messages

    def send_agent_control_state(self, id, d_x, d_y, d_xa, d_za, jetpack, brake):
        d = {
            'cmd' : 'agent_control_state',
            'id' : id,
            'tick' : 0,
            'state': [d_x, d_y, d_xa, d_za, jetpack, brake]
           }
        self.send_json(d)

    def send_chat(self, d):
        d['cmd'] = 'chat'
        self.send_json(d)

    def send_client_id(self):
        d = {
            'cmd' : 'send_client_id',
            'id' : ClientGlobal.client_id,
           }
        self.send_json(d)

    def request_chunk_list(self):
        d = {
            'cmd' : 'request_chunk_list',
            }
        self.send_json(d)

    def request_chunk(x,y,z):
        d = {
            'cmd' : 'request_chunk',
            'value' : (x,y,z)
        }

class SendAdminMessage:

    @classmethod
    def init(self):
        pass
    def __init__(self, client):
        self.client = client
        ClientGlobal.sendAdminMessage = self
## Admin messages

    def set_map(self,x,y,z,value):
        d = {
            'set_map' : 'set_map',
            'list' : [(x,y,z,value)],
            }
        send_json(d)

    def set_map_bulk(self, list): #takes a list of 4 tuples of (x,y,z,value)
        d = {
            'set_map' : 'set_map',
            'list' : list,
            }
        send_json(d)

class MessageHandler:
    def __init__(self, player):
        self.player = player
        ClientGlobal.messageHandler = self

    def process_json(self, msg):
        cmd = msg.get('cmd', None)
        if cmd is None:
            return
        if cmd == 'agent_position':
            self._agent_position(**msg)
        elif cmd == 'send_client_id':
            self._set_client_id(**msg)
        elif cmd == 'chunk_list':
            print "Chunk List Received"
            print str(msg['list'])

        elif cmd == 'chat':
            ClientGlobal.chat.receive(msg)

        else:
            print "JSON message type unregonized"

    def _agent_position(self, id, tick, state, **misc):
        [x,y,z,vx, vy, vz,ax, ay, az] = state
        [x,y,z] = [float(x),float(y),float(z)]

        self.player.x = x
        self.player.y = y
        self.player.z = z
        self.player.vx = vx
        self.player.vy = vy
        self.player.vz = vz
        self.player.ax = ax
        self.player.ay = ay
        self.player.az = az

    def _set_client_id(self, id, **misc):
        print "Received Client Id: %i" % (id,)
        if ClientGlobal.client_id == 0:
            ClientGlobal.client_id = id
            ClientGlobal.sendMessage.send_client_id()
