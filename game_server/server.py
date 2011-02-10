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
from message_listener impotr * 

class Server:

	def __init__(self, world_id):
		#server globals
		self.world_id = world_id
		self.unique_id = 1
		#
		self.message_listener = Message_lister()
		self.message_handlers = Message_handlers()
		self.info_commands = Info_commands()
		self.agent_commands = Agent_commands()
		self.admin_commands = Admin_commands()
		#game state
		self.world_map = World_map()
		self.agents = Agents()
		#self.objects = Objects()

	def share_state(self):
		excluded_0 = [] #do not include these into namespace of these classes
		excluded_1 = [] #do not include these into any other object's namespace 
		for name, object in self.__dict__.items():
			if name in excluded_0
				continue
			print 'Assigning to ', name, object
			for name2, object2 in self.__dict__.items():
				if name2 not in object.__dict__.keys():
					print 'skipping ',name2
					continue
				elif name == name2 or name2 in excluded_1:
					continue
				print 'Assigning ',name2, object2, 'to ', name, object
				object.__dict__[name2] = object2
				print 'Object ', name, 'property ', name2, ' value ',object.__dict__[name2]

	#def share_state(self):
		#not_singletons = ['pygame','handlers']
		#to_share = [singleton for singleton in self.__dict__.items() if singleton[0] not in not_singletons]

		#def share(a,b):
			#name1, object1 = a[0], a[1]
			#name2, object2 = b[0], b[1]
			#if getattr(object1,name2,'xx') != 'xx':
				#print 'Assigning',name2,'to',name1
				#setattr(object1,name2,object2)

		#[[share(singleton1,singleton2) for singleton2 in to_share] for singleton1 in to_share]
                             
	def run():
		self.share_state()
		self.message_handlers.define_handlers()
		self.message_listener.start()

if __name__ == '__main__':
	server = Server(0) #world _id = 0
	server.run()
