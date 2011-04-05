import Queue
from threading import Thread
import redis.client
import simplejson
import sys

def delta_out_worker(delta_out_q, world_id):
	out = redis.Redis("localhost") #will eventually need multiple output buffer servers
	while True:
		try:
			msg = delta_out_q.get()
			#print "delta_out_worker: " + str(msg) # DEBUGGING
			msg = simplejson.dumps(msg)
			print "delta_out_worker: " + msg # DEBUGGING
			key = "world_%s_out" % (world_id)
			num_subs = out.publish(key, msg)
			
#			if num_subs == 0:
#				print "delta_out_worker: No servers were listening"
#			else:
#				print "delta_out_worker: %i servers were listening" % (num_subs)
				
 			delta_out_q.task_done()
		except Exception, err:
			print "redis_delta_worker: Error In Worker Process,  %s: %s" %(sys.stderr, err)
			continue

class Delta: #non-blocking client notification
	
	delta_out_q = None
	def __init__(self):
		self.globals = None
		pass
		
	def start(self, world_id):
		self.delta_out_q = Queue.Queue() 
		num_worker_threads = 1
		for i in range(num_worker_threads):
			t = Thread(target=delta_out_worker, args=(self.delta_out_q, self.globals.world_id,))
			t.daemon = True
			t.start()
	
	## Agent Action relatd ##
	def meta(self, type, meta):
		msg = {}
		msg['msg'] = type
		for i,x in meta.items():
			msg[i] = x		
		self.delta_out_q.put(msg)
		
    #agent notifications
	def agent_position_change(self, agent_id, position, player_id=0): #called when agent changes position
		msg = {}
		msg['msg'] = 'agent_position_change'
		msg['id'] = agent_id
		msg['position'] = position #[0, x, y, z]
		self.delta_out_q.put(msg)
		pass

	def agent_state_change(self, agent_id, position, version, meta=None):
		msg = {}
		msg['msg'] = 'agent_state_change_update'
		msg['id'] = agent_id
		msg['version'] = version
		msg['position'] = position # [0,x,y,z]
		msg['meta'] = meta
		self.delta_out_q.put(msg)
		pass
				
	def agent_delete(self, agent_id):
		msg = {}
		msg['msg'] = 'agent_delete'
		msg['id'] = agent_id
		self.delta_out_q.put(msg)
		pass
	
	def agent_create(self, id, position, player_id=0):
		msg = {}
		msg['msg'] = 'agent_create'
		msg['id'] = id
		msg['position'] = position #(0,x,y,z)
		msg['player_id'] = player_id
		self.delta_out_q.put(msg)
		#print 'agent_create_update put'
		pass
	
	def attack_log(self, agent_id, target_id, damage, message = ""):
		msg = {}
		msg['msg'] = 'attack_log'
		msg['agent_id'] = agent_id
		msg['target_id'] = target_id
		msg['damage'] = damage
		msg['message'] = message
		self.delta_out_q.put(msg)
		#print 'agent_create_update put'
		pass		
	#object notifications
	
	#USE THIS INSTEAD
	def object_position_change(self, object_id, position, meta = None):
		msg = {}
		msg['msg'] = 'object_position_change'
		msg['id'] = object_id
		msg['position'] = position
		if meta != None:
			msg['meta'] = meta
		#msg['object_type'] = self.objects.objects[object_id]['object_type']
		self.delta_out_q.put(msg)
	
	#does not work when object is in inventory
	def object_state_change(self, object_id, position, version):
		msg = {}
		msg['msg'] = 'object_state_change'
		msg['id'] = object_id
		msg['version'] = version
		msg['position'] = position
		self.delta_out_q.put(msg)
		pass
		
	#this has problems if the object is in inventory
	def object_delete(self, object_id):
		msg = {}
		msg['msg'] = 'object_delete'
		msg['id'] = object_id
		self.delta_out_q.put(msg)
		pass
	
	#object type is a list of types
	def object_create(self, object_id, position, type):
		msg = {}
		msg['msg'] = 'object_create'
		msg['id'] = object_id
		msg['type'] = type
		msg['position'] = position
		self.delta_out_q.put(msg)

	#map notifications
	def set_map(self, x, y, z, value, meta = None): #called when map tile is changed
		msg = {}
		msg['msg'] = 'set_terrain_map'
		msg['value'] = value
		msg['x'] = x
		msg['y'] = y
		msg['z'] = z
		if meta != None:
			msg['meta'] = meta
		self.delta_out_q.put(msg)
		pass

#### JOB stuff

	def create_job(self, player_id, job_id, job_meta): #variable output
		msg = {}
		msg['msg'] = 'create_job'
		msg['player_id'] = player_id
		msg['job_id'] = job_id
		if job_meta != None:
			msg['job_meta'] = job_meta
		self.delta_out_q.put(msg)
		pass

	def delete_job(self, player_id, job_id):
		msg = {}
		msg['msg'] = 'delete_job'
		msg['player_id'] = player_id
		msg['job_id'] = job_id
		self.delta_out_q.put(msg)
		pass

	def modify_job(self, player_id, job_id, job_meta):
		msg = {}
		msg['msg'] = 'modidy_job'
		msg['player_id'] = player_id
		msg['job_id'] = job_id
		if job_meta != None:
			msg['job_meta'] = job_meta
		self.delta_out_q.put(msg)
		pass
