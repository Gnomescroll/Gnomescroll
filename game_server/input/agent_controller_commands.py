class Agent_controller_commands:
	
	def __init__(self):
		self.agent_controller = None
		self.world_map = None
		#self.agents = None
		#self.objects = None
		self.globals = None


	# required = ['position']
	# optional = ['player_id']	
	def move_goal(self, agent_id, position, **msg):
		(x,y,z) = position
		self.agent_controller.create_move_goal(agent_id, x, y, z)
	# required = ['position']
	# optional = ['player_id']
