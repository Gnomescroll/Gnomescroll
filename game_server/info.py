import Queue
from threading import Thread
import redis.client
import json
import sys

#output block, so may need more worker threads for performance
def info_out_worker(info_out_q):
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
		#self.agents = None
		#self.objects = None
		#self.map = None

		self.info_out_q = Queue.Queue() 
		num_worker_threads = 1
		for i in range(num_worker_threads):
			t = Thread(target=info_out_worker, args=(self.info_out_q,))
			t.daemon = True
			t.start()
	
	#is it really required to return the client id?
	def agent_info(self, agent_id, client_id):
		msg = {}
		msg['msg'] = 'agent_info'
		msg['agent_id'] = agent_id
		msg['client_id'] = client_id
		msg['value'] = self.agents.agents[agent_id].serialize()   #SCOPE
		self.info_out_q.put((client_id,msg))
		pass
		
	def object_info(self, object_id, client_id):
		msg = {}
		msg['msg'] = 'object_info'
		msg['object_id'] = object_id
		msg['client_id'] = client_id
		msg['object_info'] = self.object.agents[object_id].serialize()   #SCOPE
		self.info_out_q.put((client_id,msg))
		pass
		
	def map_chunk_agent_list_info(self, cx, cy, client_id, **extra):
		msg = {}
		msg['msg'] = 'map_chunk_agent_list_info'
		msg['client_id'] = client_id
		msg['cx'] = cx
		msg['cy'] = cy
		agent_list = []
		for agent in self.agents.agents.values():
			#print "agents= "+ str(agent.serialize())  #debugging
			(gx,gy,gz) = agent.position()
			(acx,acy) = self.map.get_chunk_key(gx,gy,gz=gz)
			if (cx,cy) == (acx,acy):
				agent_list.append(agent.serialize())
		msg['agent_list'] = agent_list
		self.info_out_q.put((client_id,msg))
		 
	def map_chunk_object_list_info(self, cx, cy, client_id, **extra):
		msg = {}
		msg['msg'] = 'map_chunk_object_list_info'
		msg['client_id'] = client_id
		msg['cx'] = cx
		msg['cy'] = cy
		object_list = []
		for object in self.objects.objects.values():
			(gx,gy,gz) = self.objects.get_position_from_location_state(object.location_state)
			(acx,acy) = self.map.get_chunk_key(gx,gy,gz=gz)
			if (cx,cy) == (acx,acy):
				object_list.append(object.serialize())
		msg['object_list'] = object_list
		self.info_out_q.put((client_id,msg))
