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

class Message_handlers:
	
	def __init__(self):
		pass
		
	def define_handlers():
		self.info_handler = {
		'get_agent' : self.info_commands.get_agent,
		'get_object' : self.info_commands.get_object,
		'get_map' : self.info_commands.get_map
		}
		
		self.agent_handler = {
		'move_0' : self.agent_commands.move_0,
		'dig' : self.agent_commands.dig,
		'construct_wall' : self.agent_commands.construct_wall
		
		'pickup_item' : self.agent_commands.pickup_item,
		'drop_item' : self.agent_commands.drop_item,
		#'plant_tree' : self.agent_actions.plant_tree,
		'till_soil' : self.agent_commands.till_soil,
		'plant_crop' : self.agent_commands.plant_crop,
		'harvest' : self.agent_commands.harvest,
		#'store_item_in_container' : self.agent_actions.store_item_in_container,
		#'remove_item_from_container' : self.agent_actions.remove_item_from_container
		}
		
		self.admin_handler = {
		'create_agent' : admin_commands.create_agent,
		'set_map' : admin_commands.set_map
		}

class Message_listener:
	
	def __init__(self):
		pass
	
	def start(self):
		t = Thread(target=self.message_worker, args= (self.world_id))
		t.daemon = True
		t.start()
	
	def message_worker(self, world_id):
		r_in = redis.Redis("localhost") #going to be local host for now
		key = "world_%s" % (str(world_id),
		while True:
			try:
				j = r_in.brpop(key)
				i=marshal.loads(j[1])
				if not i:
					continue
				cmd = i['cmd']
				type = i['type']
				if type == "info":
					print 'info: ',self.message_handlers.info_handler[cmd]
					pass
				if type == "agent":
					print 'agent: ',self.message_handlers.agent_handler[cmd]
					pass
				if type == "admin":
					print 'admin: ',self.message_handlers.admin_handler[cmd]
					pass
				#print 'msg_to_function',self.handlers.msg_to_function[cmd]
				self.handlers.msg_to_function[cmd](**i)
				
			except Exception, err:
				print "message_worker: %s: %s" %(sys.stderr, err)
				continue

class Server:

	def __init__(self, world_id):
		self.world_id = world_id
		#ok
		self.message_listener = Message_lister()
		self.message_handlers = Message_handlers()
		self.info_commands = Info_commands()
		self.agent_commands = Agent_commands()
		self.admin_commands = Admin_commands()
		#test
		self.world_map = World_map()
	
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

	def run():
		self.share_state()
		self.message_handlers.define_handlers()
		self.message_listener.start()

if __name__ == '__main__':
	server = Server(0) #world _id = 0
	server.run()
