from interface.agent import Agent

class Agent_commands:
	
	def __init__(self):
		self.globals = None

	# required = ['dp', agent_id']
	# optional = ['player_id']		
	def move_0(self, agent_id, dp, **msg):
		(dx, dy, dz) = dp
		Agent(agent_id).move_0(dx,dy,dz)
		pass
		
	def dig(self, **msg):
		pass
		
	def construct_wall(self, **msg):
		pass

	# required = ['agent_id', 'item_id']
	# optional = []	
	def pickup_item(self, agent_id, item_id, **msg):
		Agent(agent_id).pickup_item(item_id)
		pass

	# required = ['agent_id']
	# optional = ['item_id']			
	def drop_item(self, agent_id, **msg):
		Agent(agent_id).drop_item()
		pass
		
	def till_soil(self, agent_id, **msg):
		Agent(agent_id).till_soil()
		pass
		
	def plant_crop(self, agent_id, seed_id = None, **msg):
		if seed_id == None:
			Agent(agent_id).plant_crop(seed_id)
		else:
			Agent(agent_id).plant_crop()
		pass
		
	def harvest_crop(self, agent_id, crop_id = 0, **msg):
		Agent(agent_id).harvest_crop(crop_id)
		pass

	def craft_item(self, agent_id, recipe, **msg):
		Agent(agent_id).craft_item(agent_id, recipe) 
