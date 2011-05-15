
from net_client import NetClientGlobal

class NetApiGlobal:
    netOut = None
    SendMessage = None
    SendAdminMessage = None

    def __init__(self):
        #NetApiGlobal.netOut = NetOut()
        NetApiGlobal.sendMessage = SendMessage()
        NetApiGlobal.SendAdminMessage = SendAdminMessage()

    @classmethod
    def init(self):
        NetApiGlobal.netOut = netOut.init()
        NetApiGlobal.sendMessage.init()
        NetApiGlobal.SendAdminMessage.init()

class NetOut:
    sendMessage = None
    @classmethod
    def init(self):
        self.sendMessage = NetClientGlobal.sendMessage
        assert self.sendMessage != None
    @staticmethod
    def send_json(dict):
        self.sendMessage.send_json(dict)
    @staticmethod
    def send_binary(msg_id, bin_sting):
        self.sendMessage.send_binary(msg_id, bin_string)


class SendMessage:
    client = None

    @classmethod
    def init(self):
        self.send =

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
