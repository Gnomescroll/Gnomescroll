
import simplejson as json
#import struct

class NetEventGlobal:
    messageHandler = None

    @classmethod
    def init_0(self):
        self.messageHandler = MessageHandler()  ##MAY CAUSE ERRORS?
    @classmethod
    def init_1(self):
        MessageHandler.init()

from world_state import WorldStateGlobal
from net_client import NetClientGlobal
from net_out import NetOut

class MessageHandler:
    player = None #move this somewhere else
    @classmethod
    def init(self):
        self.player = WorldStateGlobal.player
        assert self.player != None
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
            self._3_map_chunk(self, datagram)
            print "Map Chunk Received"
        else:
            print "MessageHandler.process_binary_event: message type unknown"
#message events
    def _3_map_chunk(self, datagram):  #move this somewhere
        print "Map Chunk Received"

    def process_json_event(self, msg_type, datagram):
        try:
            msg = json.loads(datagram)
        except:
            print "MessageHandler.process_json_event error"
            print str(msg)
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
            print "process_json_event: Fix chat message callback!"
            #chat.receive(msg)
        else:
            print "JSON message type unregonized"

    def _agent_position(self, id, tick, state, **misc):
        self.player = WorldStateGlobal.player
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
        if NetClientGlobal.client_id == 0:
            NetClientGlobal.client_id = id
            NetOut.sendMessage.send_client_id()
