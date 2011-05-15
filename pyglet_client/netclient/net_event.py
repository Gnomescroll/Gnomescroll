
import simplejson as json
#import struct

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

### handling code for json/binary messages
## DEPRECATE ###
class DEPCREATE:
    def deprecate(self):
        if msg_type == 0:
            print "test message received"
        elif msg_type == 1: #json
            self._1_json(datagram)
        elif msg_type == 3:
            self._3_map_chunk(datagram)
        else:
            print "unknown message type: %i" % msg_type

    def _1_json(self, datagram):
        try:
            msg = json.loads(datagram)
        except:
            print "error decoding: len = %i, message_length= %i" % (len(datagram), length)
            msg = { 'cmd' : 'error' }
        self.messageHandler.process_json(msg)

    def _3_map_chunk(self, datagram):
        print "Map Chunk Received"

## DEPRECATE ##


class MessageHandler:
    def __init__(self, player):
        self.player = player
        ClientGlobal.messageHandler = self

    def process_net_event(self, msg_type, datagram):
        if msg_type == 1: #json message
            self.process_json_event(datagram)
        else:  #create a process json message
            self.process_binary_event(msg_type, datagram)

    def process_binary_event(self, msg_type, datagram):
        if msg_type == 3:
            self._3_map_chunk(self, datagram):
            print "Map Chunk Received"

    def _3_map_chunk(self, datagram):
        print "Map Chunk Received"

    def process_json_event(datagram):
        try:
            msg = json.loads(datagram)
            self.messageHandler.process_json(msg)
        except:
            print "MessageHandler.process_json_event"
            return
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
