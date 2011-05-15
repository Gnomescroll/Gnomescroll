
class ClientEventGlobal:
    clientEvent = None

    def init(self):
        ClientEventGlobal.clientEvent = ClientEvent

class ClientEvent:

    @classmethod
    def init_0(self): #do instantiations here
        pass
    @classmethod
    def init_1(self): #do imports heres
        pass

    ## event callbacks
    def client_connected():
        pass
