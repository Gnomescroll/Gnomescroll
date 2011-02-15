import redis.client
import marshal
import time
import json
import Queue
from threading import Thread
import itertools

#game state classes
from game_server.state.world_map import *
from game_server.state.agents import *
from game_server.state.objects import *
from game_server.state.world_time import *
from game_server.state.globals import *

##message handling
from game_server.input.info_commands import *
from game_server.input.agent_commands import *
from game_server.input.admin_commands import *
from game_server.input.message_handlers import *
from game_server.input.message_listener import *
 
from game_server.interface.agent import *
from game_server.interface.object import *

#interface helpers
from game_server.interface.message_handlers import *

class Server:

	def __init__(self, world_id):
		# listeners for input
		self.message_listener = Message_lister()
		self.message_handlers = Message_handlers()
		self.info_commands = Info_commands()
		self.agent_commands = Agent_commands()
		self.admin_commands = Admin_commands()
		# output to server
		self.delta = Delta()
		self.info = Info()
		#game state
		self.world_map = World_map()
		self.world_time = World_time()
		self.agents = Agents()
		self.globals = Globals()
		self.objects = Objects()

	def share_state(self):
		not_singletons = []
		to_share = [singleton for singleton in self.__dict__.items() if singleton[0] not in not_singletons]
		def share(a,b):
			name1, object1 = a[0], a[1]
			name2, object2 = b[0], b[1]
			if getattr(object1,name2,'xx') != 'xx':
				print 'Assigning',name2,'to',name1
				setattr(object1,name2,object2)
		[[share(singleton1,singleton2) for singleton2 in to_share] for singleton1 in to_share]
		
		### INIT Templates ###
		#Agent class init
		Agent.world_map = self.world_map
		Agent.delta = self.delta
		Agent.agents = self.agents.agents
		#Object class init
		Nobject.world_map = self.world_map
		Nobject.delta = self.delta
		Nobject.objects = self.entity.objects

	def run():
		self.share_state()
		self.world_map.init() #load map
		self.agents.init() #load agents
		#self.entities.init() #load objects
		self.world_time.start()
		self.message_handlers.define_handlers()
		self.message_listener.start()

if __name__ == '__main__':
	server = Server(0) #world _id = 0
	server.run()
