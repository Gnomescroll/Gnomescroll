import init_c_lib
import c_lib.terrain_map as cMap

class NetServer:
    connectionPool = None

    @classmethod
    def init(cls):
        cls.connectionPool = PyClientPool()

class PyClient:

    def __init__(self, client_id):

        self.client_id = client_id
        self.id = client_id
        print "PyClient initialized with client_id %d" % (client_id,)

        self.loaded_once = False
        init_c_lib.create_agent(self.client_id)
        init_c_lib.send_id_to_client(self.client_id)
        init_c_lib.join_chat(self.client_id)

    def get_name(self):
        if not init_c_lib.client_identified(self.client_id):
            return None
        return init_c_lib.get_agent_name(self.client_id)

    def ready(self):
        if not init_c_lib.client_identified(self.client_id) or self.loaded_once:
            return
        print "Client is ready"
        self.loaded_once = True
        init_c_lib.send_game_state(self.client_id)
        
class PyClientPool:

    def __init__(self):
        self.clients_by_id = {}    
        #register_client_creation(( lambda _client_id: self.add_client(_client_id) ))
        #register_client_deletion(( lambda _client_id: self.remove_client(_client_id) ))


    #called when client connection established
    def add_client(self, _client_id):
        print "python add_client callback: client_id %d" % (_client_id,)
        client =  PyClient(_client_id)
        self.clients_by_id[_client_id] = client
        print "PyClientPool: connection associated with client_id= %s" % (_client_id)

    #called on connection deconstruction
    def remove_client(self, client_id):
        client = self.clients_by_id[client_id]

        # dispatch event
        init_c_lib.leave_chat(client_id)
        init_c_lib.leave_team(client_id)
        init_c_lib.destroy_agent(client_id)
        
        # remove from registry
        del self.clients_by_id[client.id]
        print "PyClientPool: remove_client, id= %i" % (client_id)

    def by_client_id(self, client_id):
        if client_id in self.clients_by_id:
            return self.clients_by_id[client_id]
        raise ValueError, "Unknown client_id %d", client_id

    def check_clients_ready(self):
        for client in self.clients_by_id.values():
            client.ready()
