
#deprecate
#class NetApiGlobal:
    #netOut = None
    #sendMessage = None
    #adminMessage = None
    #chatMessage = None

    #def __init__(self):
        ##NetApiGlobal.netOut = NetOut()
        #NetApiGlobal.sendMessage = SendMessage()
        #NetApiGlobal.sendAdminMessage = SendAdminMessage()
        #NetApiGlobal.sendChatMessage = SendChatMessage()

    #@classmethod
    #def init(self):
        #NetApiGlobal.netOut = netOut.init()

class NetOut:
    sendMessage = None

    sendMessage = None
    adminMessage = None
    chatMessage = None

    @classmethod
    def init_0(self):
        NetOut.sendMessage = NetClientGlobal.sendMessage
        NetOut.sendMessage = SendMessage()
        NetOut.adminMessage = AdminMessage()
        NetOut.chatMessage = ChatMessage()
    @clasmethod
    def init_1(self):
        pass
    @staticmethod
    def send_json(dict):
        self.sendMessage.send_json(dict)
    @staticmethod
    def send_binary(msg_id, bin_sting):
        self.sendMessage.send_binary(msg_id, bin_string)

from net_client import NetClientGlobal

class SendMessage:

    def send_agent_control_state(self, id, d_x, d_y, d_xa, d_za, jetpack, brake):
        d = {
            'cmd' : 'agent_control_state',
            'id' : id,
            'tick' : 0,
            'state': [d_x, d_y, d_xa, d_za, jetpack, brake]
           }
        NetOut.send_json(d)

    def send_client_id(self):
        d = {
            'cmd' : 'send_client_id',
            'id' : ClientGlobal.client_id,
           }
        NetOut.send_json(d)

    def request_chunk_list(self):
        d = {
            'cmd' : 'request_chunk_list',
            }
        NetOut.send_json(d)

    def request_chunk(x,y,z):
        d = {
            'cmd' : 'request_chunk',
            'value' : (x,y,z)
        }

class ChatMessage:

    def send_chat(self, d):
        d['cmd'] = 'chat'
        NetOut.send_json(d)

class AdminMessage:

    def set_map(self,x,y,z,value):
        d = {
            'set_map' : 'set_map',
            'list' : [(x,y,z,value)],
            }
        NetOut.send_json(d)

    def set_map_bulk(self, list): #takes a list of 4 tuples of (x,y,z,value)
        d = {
            'set_map' : 'set_map',
            'list' : list,
            }
        NetOut.send_json(d)
