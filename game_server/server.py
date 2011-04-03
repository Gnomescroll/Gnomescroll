#import redis.client
#import marshal
import time
#import json
#import Queue
#from threading import Thread
#import itertools

#game state classes
from state.world_map import World_map
from state.world_time import World_time
from state.globals import Globals
from state.agents import Agents
from state.objects import Objects
from state.dat import Dat
from state.agent_command_scheduler import Agent_command_scheduler

#agent controller class
from agent_controller.agent_controller import Agent_controller
from agent_controller.job_manager import Job_manager

##input message handling
from input.message_listener import Message_listener
from input.message_handlers import Message_handlers
from input.info_commands import Info_commands
from input.agent_commands import Agent_commands
from input.admin_commands import Admin_commands
from input.agent_controller_commands import Agent_controller_commands
from input.job_manager_commands import Job_manager_commands
## output messages
from output.info import Info
from output.delta import Delta

#interfaces 
from interface.agent import Agent
from interface.object import Nobject
from interface.crop import Crop

class Server:

    def __init__(self, world_id):
        self.globals = Globals(world_id) #takes world_if as input
        # listeners for input
        self.message_listener = Message_listener()
        self.message_handlers = Message_handlers()
        self.info_commands = Info_commands()
        self.agent_commands = Agent_commands()
        self.admin_commands = Admin_commands()
        self.agent_controller_commands = Agent_controller_commands()
        self.job_manager_commands = Job_manager_commands()
        # output to server
        self.info = Info()
        self.delta = Delta()
        #game state
        self.world_map = World_map()
        self.world_time = World_time()
        self.agents = Agents()
        self.objects = Objects()
        self.dat = Dat()
        self.agent_command_scheduler = Agent_command_scheduler()
        #agent controller
        self.agent_controller = Agent_controller()
        self.job_manager = Job_manager()

    def share_state(self):
        print "Share State Start"
        not_singletons = []
        to_share = [singleton for singleton in self.__dict__.items() if singleton[0] not in not_singletons]
        def share(a,b):
            name1, object1 = a[0], a[1]
            name2, object2 = b[0], b[1]
            if getattr(object1,name2,'xx') != 'xx':
                #print 'Assigning',name2,'to',name1
                setattr(object1,name2,object2)
        [[share(singleton1,singleton2) for singleton2 in to_share] for singleton1 in to_share]

        ### INIT Templates ###
        #Agent class init
        Agent.world_map = self.world_map
        Agent.delta = self.delta
        Agent.agents = self.agents
        Agent.objects = self.objects
        Agent.world_time = self.world_time
        Agent.dat = self.dat
        #Object class init
        Nobject.world_map = self.world_map
        Nobject.delta = self.delta
        Nobject.agents = self.agents
        Nobject.objects = self.objects
        Nobject.world_time = self.world_time
        Nobject.dat = self.dat
        #Crop class init
        Crop.world_map = self.world_map
        Crop.delta = self.delta
        Crop.agents = self.agents
        Crop.objects = self.objects
        Crop.world_time = self.world_time
        Crop.dat = self.dat
        
    def run(self):
        self.share_state()
        self.info.start(self.globals.world_id)
        self.delta.start(self.globals.world_id)
        self.world_map.init() #load map
        self.agents.init() #load agents
        #self.entities.init() #load objects
        self.world_time.start()
        self.message_handlers.define_handlers()
        #self.message_listener.start()
        self.message_listener.single_threaded_start()
        
if __name__ == '__main__':
    server = Server(0) #world _id = 0
    server.run()
    time.sleep(3600)
