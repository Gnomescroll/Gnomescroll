

job_validation = {
	'construct_tile' : { 
		'tile_id' : 'tile_id',
		'material_list' : 'list of item_id',
		'build_from' : 'location',
		'build_at' : 'location'
	},
	
	
}

class Job_manager:

	def __init__(self):
		self.job_pool = {}
		self.globals = None
		self.dat = None
		self.delta = None

	def job_id(self):
		return self.globals.get_unique_id()

### Job control functionality

	def add_job(self, player_id, job):
		if job['type'] == 'construct_tile':
			self.construct_tile(player_id, **job)
		else:
			print "job_manager.add_job: invalid job type"
			
	def delete_job(self, player_id, job_id):
		del self.job_pool[job_id]
		self.delta.delete_job(player_id, job_id)
		
##individual jobs
	
	def construct_tile(self, player_id, tile_name, material_list, build_from, build_at, **misc):
		
		tile_id = self.dat.tile_name_to_id(tile_name)
		
		job_id = self.job_id()
		job = {
			'job_id' : job_id,
			'job_type' : 'construct_tile',
			'player_id' : player_id,
			'agent_id' : 0,

			'status' : '-',
			'current_task' : 0,

			'tasks' : None,
			'num_tasks' : None
			
		}
		
		tasks = []
		## move each item to build location
		for item_id in material_list:
			tasks.append([
					('::move_item', item_id, build_from),
					])
		
		##  build wall
		tasks.append([
				('::move', build_from),
				(':construct_tile', tile_id, build_at),
			])
		
		job['num_tasks'] = len(tasks)
		job['tasks'] = tasks
		self.job_pool[job_id] = job
		self.delta.create_job(player_id, job_id, job)
	
	#agent claims job and sub-job task script is released
	def claim_job(self, agent_id, job_id):
		print "job: agent_claim_job, " + str((agent_id, job_id))
		job = self.job_pool[job_id]
		#sanity checks
		if job['agent_id'] != 0:
			print "job_manager agent_claim_job error: job is held by another agent"
			return (None, None, None)
		#end
		job['agent_id'] = agent_id
		
		current_task = job['current_task']
		script = job['tasks'][current_task]
		
		self.delta.modify_job(job['player_id'], job_id, job)
		
		return (script,job_id,current_task)

	def agent_release_job(self, job_id, agent_id = None):
		print "job: agent_release_job, " + str((job_id, agent_id))
		job = self.job_pool[job_id]
		#sanity checks
		if agent_id != None:
			if agent_id != job['agent_id']:
				print "job_manager agent_release_job error: agent releasing job does not match agent grabbbing job"
		#end
		job['agent_id'] = 0
		self.delta.modify_job(job['player_id'], job_id, job)
		pass
		
	def complete_sub_task(self, job_id, task_num): 
		print "job: complete_subtask, " + str((job_id, task_num))		
		job = self.job_pool[job_id]
		if task_num != 	job['current_task']:
			print "job_manager complete_sub_task error: jobs subtasks do not match"

		job['current_task'] = job['current_task'] + 1 #increment
		if job['current_task'] == job['num_tasks']:
			print "Job Finished"
			job['status'] = 'completed'
			#self.delta.modify_job(player_id, job_id, job)
			return None
		else:
			print "More Job Tasks Left"
			#self.delta.modify_job(player_id, job_id, job)
			return 1
	
	def next_sub_job(self, job_id):

		job = self.job_pool[job_id]		
		current_task = job['current_task']
		script = job['tasks'][current_task]
			
		return {
			'script' : script.copy(),
			'job_id' : job_id,
			'task_num' : current_task, 
			}
		
	def assign_agent_job(self, agent_id, job_id):
		pass
