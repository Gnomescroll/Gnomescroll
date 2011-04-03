

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
	
	def construct_tile(self, player_id, tile_id, material_list, build_from, build_at, **misc):
		job_id = self.job_id()
		job = {
			'job_id' : job_id,
			'job_type' : 'construct_tile',
			'player_id' : player_id,
			'agent_id' : 0,
			'sub_task' : 0,
			'stages' : {},
			'num_stages' : 0
			
		}
		num = 0 
		
		## move each item to build location
		for item_id in material_list:
			job['stages'][num] =	{
					'status' : '-',
					'completed' : 0,
					'script' : [
					('::move_item', item_id, build_from),
					],
			},
			num = num + 1
		
		##  build wall
		job['stages'][num] = 	{
			'status' : '-',
			'completed' : 0,
			'script' : [
				('::move', build_from),
				(':construct_tile', tile_id, build_at),
			],
		}
		job['num_stages'] = num
		self.job_pool[job_id] = job
		self.delta.create_job(player_id, job_id, job)
	
	#agent claims job and sub-job task script is released
	def agent_claim_job(self, agent_id, job_id):
		job = self.job_pool[job_id]
		#sanity checks
		if job['agent_id'] != 0:
			print "job_manager agent_claim_job error: job is held by another agent"
			return 0
		#end
		job['agent_id'] = agent_id
		sub_task = job['sub_task']
		task = job['stages'][sub_task]
		script = task['script']
		return {
		'script' : script.copy(),
		'job_id' : job_id,
		'sub_task' : sub_task, 
		}

	def agent_release_job(self, job_id, agent_id = None):
		job = self.job_pool[job_id]
		#sanity checks
		if agent_id != None:
			if agent_id != job['agent_id']:
				print "job_manager agent_release_job error: agent releasing job does not match agent grabbbing job"
		#end
		job['agent_id'] = 0
		self.delta.modify_job(job['player_id'], job_id, job)
		pass
		
	def complete_sub_task(self, job_id, sub_task): 
		job = self.job_pool[job_id]
		if sub_task != 	job['sub_task']:
			print "job_manager complete_sub_task error: jobs subtasks do not match"
		stage = job['stages'][sub_task] 
		stage['completed'] = 1
		stage['status'] = 'completed'
		job['sub_task'] = job['sub_task'] + 1 #increment
		if job['sub_task'] == job['num_stages']:
			print "Job Finished"
			return 0
		else:
			print "More Job Tasks Left"
			return job_id

		self.delta.modify_job(player_id, job_id, job)
		
	def assign_agent_job(self, agent_id, job_id):
		pass
