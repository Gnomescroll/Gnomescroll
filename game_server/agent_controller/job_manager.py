

job_validation = {
	'construct_tile' : { 
		'tile_id' : 'tile_id',
		'material_list' : 'list of item_id',
		'build_from' : 'location',
		'build_at' : 'location'
	},
	
	
}

class job_manager:
	
	self.globals = None
	self.dat = None
	self.delta = None

	def __init__(self):
		self.job_pool = {}
		
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
	
	def construct_tile(self, player_id, tile_id, material_list, build_from, build_at):
		##validation
		
		
		job_id = self.job_id()
		job = {
			'player_id' : ''
			'status' : 'active',
		
		}
		

class job_list:

	self.globals = None
	
	def __init__(self):
		self.job_id = 0
		self.job_pool = {}

	def get_id(self):
		id = self.job_id
		self.job_id = self.job_id + 1
		return id
 
	def add_job(self, task_name):
		pass

	def remove_job(self, id):
		pass
	
	def delegate_agent(self, task_id, agent_id):
		pass


class job_entry:
	self.task_manager = None
	self.dat = None

	def __init__(self):
		this.agent_id = 0
		this.job_type = []

		this.task = 

### define sequence of commands
### determine state requirements

	def build_wall(job_id):
	#check requirements / parameters
	action = {
	'action' : 'move',
	'position' : (0, 2, 2, 2)
	}
	#move agent to rock
	#move rock to build location
	#build
	
	
	
#create task
#modify task
