from incoming.message_handlers import Message_handlers
from incoming.server_listener import Server_listener

from incoming.terrain_map_handler import Terrain_map_handler
from incoming.agent_handler import Agent_handler
from incoming.object_handler import Object_handler

from state.agents import Agents
from state.objects import Objects
from state.terrain_map import Terrain_map
from state.globals import Globals

from server_API import *

import time

admin = Admin_commands()
agent = Agent_commands()
info = Info_commands()


class Client:
	
	def __init__(self, world_id):
		#listener 
		self.server_listener = Server_listener()
		self.message_handlers = Message_handlers()	
		#messages handles
		self.terrain_map_handler = Terrain_map_handler()
		self.agent_handler = Agent_handler()
		self.object_handler = Object_handler()
		#state
		self.agents = Agents()
		self.objects = Objects()
		self.globals = Globals(world_id) #world id zero
		self.terrain_map = Terrain_map

	def setup(self):
		self.share_state()
		self.server_listener.start()
		#load map
		info.get_map(0)
		#load agents
		info.get_agent_list()
		#load objects
		info.get_object_list()

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


if __name__ == '__main__':
	client = Client(0) #world _id = 0
	client.setup()
	#Do something!
	#debugging
	from pudb import set_trace; set_trace()
	time.sleep(3600)
