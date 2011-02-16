from interface.agent import Agent

class Agent_commands:
	
	def __init__(self):
		self.globals = None

	# required = ['dp, agent_id']
	# optional = ['player_id']		
	def move_0(self, agent_id, dp, **msg):
		(dx, dy, dz) = dp
		Agent(agent_id).move_0(dx,dy,dz)
		pass
		
	def dig(self, **msg):
		pass
		
	def construct_wall(self, **msg):
		pass

	def pickup_item(self, **msg):
		pass
		
	def drop_item(self, **msg):
		pass
		
	def till_soil(self, **msg):
		pass
		
	def plant_crop(self, **msg):
		pass
		
	def harvest(self, **msg):
		pass
