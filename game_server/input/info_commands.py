#imports

class Info_commands:
    
    def __init__(self):
        self.info = None
        self.globals = None

    #required = ['agent_id', 'client_id', 'world_id']
    #optional = []      
    def get_agent(self, agent_id=None, client_id=0, **msg):
        if agent_id is None:
            return
        self.info.agent_info(agent_id, client_id)

    #required = ['object_id', 'client_id', 'world_id']
    #optional = []          
    def get_object(self, object_id=None, client_id=0, **msg):
        if object_id is None:
            return
        self.info.object_info(object_id, client_id)

    #required = ['client_id', 'world_id']
    #optional = ['z']
    def get_map(self, client_id=0, z=0, **msg):
        self.info.get_map(z, client_id)

    #required = ['client_id', 'world_id']
    #optional = []
    def get_agent_list(self, client_id=0, **msg):
        self.info.get_agent_list(client_id)

    def get_object_list(self, client_id=0, **msg):
        self.info.get_object_list(client_id)

    def get_tiles(self, client_id=0, **msg):
        self.info.get_tiles(client_id)
