import Queue
from threading import Thread
import redis.client
import json
import sys

def delta_out_worker(delta_out_q, world_id):
	out = redis.Redis("localhost") #will eventually need multiple output buffer servers
	while True:
		try:
			msg = delta_out_q.get()
			msg = json.dumps(msg)
			#print "delta_out_worker: " + msg # DEBUGGING
			key = "world_%s_out" % (world_id)
			num_subs = out.publish(dest, msg)
			if num_subs == 0:
				print "delta_out_worker: No servers were listening"
			else:
				print "delta_out_worker: %i servers were listening" % (num_subs)
 			delta_out_q.task_done()
		except Exception, err:
			print "redis_delta_worker: Error In Worker Process,  %s: %s" %(sys.stderr, err)
			continue

class Delta: #non-blocking client notification
	
	delta_out_q = None
	def __init__(self):
		self.delta_out_q = Queue.Queue()
		num_worker_threads = 1
		for i in range(num_worker_threads):
			t = Thread(target=delta_out_worker, args=(self.delta_out_q,))
			t.daemon = True
			t.start()
			
    #agent notifications
	def agent_position_change(self, agent_id, x, y, z, player_id): #called when agent changes position
		msg = {}
		msg['msg'] = 'agent_position_update'
		msg['id'] = id
		msg['x'] = x
		msg['y'] = y
		msg['z'] = z
		#msg['player_id'] = player_id
		self.delta_out_q.put(msg)
		pass

	def agent_state_change(self, agent_id, gx, gy, gz):
		msg = {}
		msg['msg'] = 'agent_state_change_update'
		msg['agent_id'] = agent_id
		msg['gx'] = gx
		msg['gy'] = gy
		msg['gz'] = gz
		self.delta_out_q.put(msg)
		pass
				
	def agent_delete(self, agent_id):
		msg = {}
		msg['msg'] = 'agent_delete_update'
		msg['agent_id'] = agent_id
		#msg['gx'] = gx
		#msg['gy'] = gy
		#msg['gz'] = gz
		self.delta_out_q.put(msg)
		pass
	
	def agent_create(self, id, x, y, z, player_id=None):
		msg = {}
		msg['msg'] = 'agent_create_update'
		msg['id'] = id
		msg['x'] = x
		msg['y'] = y
		msg['z'] = z
		msg['player_id'] = player_id
		self.delta_out_q.put(msg)
		#print 'agent_create_update put'
		pass
		
	#object notifications
	
	#USE THIS INSTEAD
	def object_location_update(self, object_id, location_state):
		msg = {}
		msg['msg'] = 'object_location_update'
		msg['object_id'] = object_id
		msg['location_state'] = location_state
		#msg['object_type'] = self.objects.objects[object_id]['object_type']
		self.delta_out_q.put(msg)
	
	#does not work when object is in inventory
	def object_state_change(self, object_id, gx, gy, gz):
		msg = {}
		msg['msg'] = 'object_state_change_update'
		msg['object_id'] = object_id
		msg['gx'] = gx
		msg['gy'] = gy
		msg['gz'] = gz
		self.delta_out_q.put(msg)
		pass
		
	#this has problems if the object is in inventory
	def object_delete(self, object_id, gx, gy, gz):
		msg = {}
		msg['msg'] = 'object_delete_update'
		msg['agent_id'] = object_id
		#msg['gx'] = gx
		#msg['gy'] = gy
		#msg['gz'] = gz
		self.delta_out_q.put(msg)
		pass
	
	def object_create(self, object_id, object_type, location_state, var_dict={}):
		msg = var_dict
		msg['msg'] = 'object_create_update'
		msg['object_id'] = object_id
		msg['object_type'] = object_type
		msg['location_state'] = location_state
		self.delta_out_q.put(msg)
