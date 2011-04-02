class Job_manager_commands:
	
	def __init__(self):
		#self.agent_controller = None
		#self.agent_command_scheduler = None
		self.job_manager = None
		#self.world_map = None
		#self.agents = None
		#self.objects = None
		#self.globals = None


	# required = ['']
	# optional = ['']	
	def create_job(self, player_id, job, **msg):
		self.job_manager.add_job(player_id, job)
		
	# required = ['']
	# optional = ['']
	def delete_job(self, player_id, job_id, **msg):
		self.job_manager.delete_job(player_id, job_id)
