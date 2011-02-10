import redis.client
import marshal
import time
import json
import Queue
from threading import Thread

from info_commands import *
from agent_commands import *
from admin_commands import *

info_commands = Info_commands()
agent_commands = Agent_commands()
admin_commands = Admin_commands()

info_handler = {
'get_agent' : info.get_agent,
'get_object' : info.get_object,
'get_map' : info.get_map
}

agent_handler = {
'move_0' : agent_commands.move_0,
'pickup_item' : agent_commands.pickup_item,
'drop_item' : agent_commands.drop_item,
#'plant_tree' : self.agent_actions.plant_tree,
'till_soil' : agent_commands.till_soil,
'plant_crop' : agent_commands.plant_crop,
'dig' : agent_commands.dig,
'construct_wall' : agent_commands.construct_wall
#'store_item_in_container' : self.agent_actions.store_item_in_container,
#'remove_item_from_container' : self.agent_actions.remove_item_from_container
}

admin_handler = {
'create_agent' : admin_commands.create_agent,
'set_map' : admin_commands.set_map
}

class message_handling:
	
	def __init__(self):
		self.world_id = 0
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
					print 'info',info_handler[cmd]
					pass
				if type == "agent":
					pass
				if type == "admin":
					pass
				#print 'msg_to_function',self.handlers.msg_to_function[cmd]
				self.handlers.msg_to_function[cmd](**i)
				
			except Exception, err:
				print "message_worker: %s: %s" %(sys.stderr, err)
				continue
