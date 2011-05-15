class NetApiGlobal:
    netOut = None

    def __init__(self):
        NetApiGlobal.netOut = NetOut()
    @classmethod
    def init(self):
        NetApiGlobal.netOut = netOut().init()


class NetOut:

    @classmethod
    def init(self):
        pass
    def __init(self):
        pass


class MoveThisToNetApi:
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
