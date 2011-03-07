from incoming.message_handlers import Message_handlers
from incoming.listener import Server_listener

from incoming.terrain_map_handler import Terrain_map_handler
from incoming.agent_handler import Agent_handler
from incoming.object_handler import Object_handler

from state.agents import Agents
from state.objects import Objects
from state.terrain_map import Terrain_map
from state.globals import Globals

from server_API import Admin_commands, Agent_commands, Info_commands


class Client:
    def __init__(self, world_id=0):
        #listener 
        self.listener = Server_listener()
        self.message_handlers = Message_handlers()    
        
        #messages handles
        self.terrain_map_handler = Terrain_map_handler()
        self.agent_handler = Agent_handler()
        self.object_handler = Object_handler()
        
        #commands
        self.admin = Admin_commands()
        self.agent = Agent_commands()
        self.info = Info_commands()
        
        #state
        self.agents = {}
        self.objects = {}
        self.globals = Globals(world_id) 
        self.terrain_map = Terrain_map()

    def setup(self):
        self.share_state()
        self.message_handlers.start()
        self.listener.start()
        
        #load map
        self.info.get_map(0)
        
        #load agents
        self.info.get_agent_list()
        
        #load objects
        self.info.get_object_list()
    
    def set_agent(self, agent):
        pass
        
    def get_agent(self, agent_id):
        pass
        
    def set_object(self, object):
        pass
        
    def get_object(self, object_id):
        pass
        
    def share_state(self):
        print "Share State Start"
        not_singletons = []
        to_share = [singleton for singleton in self.__dict__.items() if singleton[0] not in not_singletons]
        def share(a,b):
            name1, object1 = a[0], a[1]
            name2, object2 = b[0], b[1]
            if getattr(object1,name2,'xx') != 'xx':
                print 'Assigning',name2,'to',name1
                setattr(object1,name2,object2)
        [[share(singleton1,singleton2) for singleton2 in to_share] for singleton1 in to_share]


if __name__ == '__main__':
    import time
    client = Client(0) #world _id = 0
    client.setup()
    #Do something!
    #debugging
    while 1:
        time.sleep(1)
        client.info.get_map(0)
        client.info.get_agent_list()
        client.info.get_object_list()
        time.sleep(5)
        print "Current Map Size: %s" %client.terrain_map.x_size
        print '\n'
