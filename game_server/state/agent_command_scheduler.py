
class Agent_command_scheduler:
	
	def __init__(self):
		globals = None
		world_time = None

		last_time = 0

		time_slots = {}
		scheuduled_agents = {}
		
	def execute(self, agent_id, agent_command, params):
		if not agent_id in scheuduled_agents: #execute immediately
			#execute immediately
			agent = Agent(agent_id)
			agent_command(agent, *params)
		else: #put command on stack
			next_move = self.next_move[agent_id]
			
	def time_step(self, current_time):
		if last_time = 0:
			return
		
		for t in range(last_time, current_time):
			if t in time_slots:
				for x in time_slots[t]:
					#process
					#call agent controller
	def schedule(self, agent_id, agent_command, params)
