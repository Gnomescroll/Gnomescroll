import Queue
from threading import Thread
import redis.client
import json
import sys

from interface.agent import Agent
from interface.object import Nobject

#output block, so may need more worker threads for performance
def info_out_worker(info_out_q, world_id):
	out = redis.Redis("localhost")
	#the output buffer may depend on the client_id in the future
	while True:
		try:
			(client_id, msg) = info_out_q.get()
			msg = json.dumps(msg)
			#print "info_out_worker: " + str(msg) # DEBUGGING
			##change in production
			#dest = "client_%s" % (str(client_id))
			dest = "world_0_out"
			num_subs = out.publish(dest, msg)
			if num_subs == 0:
				print "info_out_worker: no subscribers"
			else:
				print "info_out_worker: " + str(num_subs) + " subscribers" 
			info_out_q.task_done()
		except:
			print "redis_info_worker: Error In Worker Process"
			continue

class Info: #non-blocking client notification
	
	def __init__(self):
		pass
		
	def start(self, world_id):
		self.info_out_q = Queue.Queue() 
		num_worker_threads = 1
		for i in range(num_worker_threads):
			t = Thread(target=info_out_worker, args=(self.info_out_q, world_id,))
			t.daemon = True
			t.start()
	
	def agent_info(self, id, client_id):
		msg = {}
		msg['msg'] = 'agent_info'
		msg['id'] = id
		msg['client_id'] = client_id
		msg['value'] = Agent(id).serialize()
		self.info_out_q.put((client_id,msg))
		pass
		
	def object_info(self, id, client_id):
		msg = {}
		msg['msg'] = 'object_info'
		msg['id'] = id
		msg['client_id'] = client_id
		msg['object_info'] = Nobject(id).serialize()
		self.info_out_q.put((client_id,msg))
		pass
	
	def get_map(z=0, client_id=0):
		array = self.world_map.get_z_level(z)
		msg = {}
		msg['map'] = array
		msg['z_level'] = z
		msg['x_size'] = self.world_map.x_size
		msg['y_size'] = self.world_map.y_size
		msg['world_id'] = self.globals.world_id
		msg['client_id'] = client_id
		self.info_out_q.put((client_id,msg))
	
	def get_agent_list(self, client_id, **extra):
		msg = {}
		msg['msg'] = 'agent_list'
		msg['world_id'] = self.globals.world_id
		msg['client_id'] = client_id
		list = []
		for id in self.agents.agents.keys():
			list.append(Agent(id).simple_serialize())
		msg['list'] = list
		self.info_out_q.put((client_id,msg))
	
	def get_object_list(self, client_id, **extra):
		msg = {}
		msg['msg'] = 'object_list'
		msg['world_id'] = self.globals.world_id
		msg['client_id'] = client_id
		list = []
		for id in self.agents.agents.keys():
			list.append(Nobject(id).simple_serialize())
		msg['list'] = list
		self.info_out_q.put((client_id,msg))
