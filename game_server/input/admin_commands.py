#imports

class Admin_commands:
	
	def __init__(self):
		pass
	
	# required = ['position']
	# optional = ['player_id']
	def create_agent(self, position, player_id = 0, **msg):
		(position_type, x, y, z) = position
		self.agents.create(x, y, z, player_id):
		pass
	
	#required ['position', 'value', 'world_id']
	#optional []
	def set_map(self, position, value, **msg):
		(type, x, y, z) = position
		self.world_map.set(x, y, z, value)
