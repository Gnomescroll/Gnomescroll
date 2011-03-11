from incoming.message_handlers import Message_handlers
from incoming.listener import Server_listener

from incoming.terrain_map_handler import Terrain_map_handler
from incoming.agent_handler import Agent_handler
from incoming.object_handler import Object_handler

#from state.agents import Agents
#from state.objects import Objects
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
    
    def update_agents(self):
        """updates self.agents based on changes to self.agent_handler.Agents"""
        """Agent_Handler should provide a dict, instead of fiddling with the same
           3 times before it's used."""
        #TODO: deletion
        for agent in self.agent_handler.agents:
            self.agents[agent['id']] = agent
    
    def move_agent(self, agent_id, x, y, z):
        """adds x, y, to agent's current x, y"""
        if agent_id in self.agents.keys():
            self.agent.move_0(agent_id, x, y, z)
        else:
            raise Exception("Can't move what I can't see.")
        

    def update_objects(self):
        #TODO: deletion
        for object in self.object_handler.objects:
            self.objects[object['id']] = object
    
    def update(self):
        self.update_agents()
        self.update_objects()
            
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
    #debugging
    while 1:
        time.sleep(1)
        client.info.get_map(0)
        client.info.get_agent_list()
        client.info.get_object_list()
        client.update()
        time.sleep(5)
        print "Current Map Size: %s" %client.terrain_map.x_size
        print '\n'
