import redis.client
import marshal
import time
import json
import Queue
from threading import Thread

from world_map import *

##message handling
from info_commands import *
from agent_commands import *
from admin_commands import *
from message_handlers import *
from message_listener import * 
from delta import *
from info import *

class Server:

	def __init__(self, world_id):
		#server globals
		self.world_id = world_id
		self.unique_id = 1
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
		self.agents = Agents()
		#self.objects = Objects()

	def share_state(self):
		not_singletons = ['pygame','handlers']
		to_share = [singleton for singleton in self.__dict__.items() if singleton[0] not in not_singletons]
		def share(a,b):
			name1, object1 = a[0], a[1]
			name2, object2 = b[0], b[1]
			if getattr(object1,name2,'xx') != 'xx':
				print 'Assigning',name2,'to',name1
				setattr(object1,name2,object2)
		[[share(singleton1,singleton2) for singleton2 in to_share] for singleton1 in to_share]
		
		#Agent class init
		Agent.world_map = self.world_map
		Agent.delta = self.delta
		Agent.agents = self.agents.agents
		#Object class init
		#Object.world_map = self.world_map
		#Object.delta = self.world_map
		#Object.objects = self.objects.objects
		           
	def run():
		self.share_state()
		self.world_map.init() #load map
		self.agents.init() #load agents
		#self.objects.init() #load objects
		self.message_handlers.define_handlers()
		self.message_listener.start()

if __name__ == '__main__':
	server = Server(0) #world _id = 0
	server.run()
