
class Delta_handler:

	def __init__(self):
		pass


	### MAP DELTA MESSAGES ###
	
	def set_terrain_map(self, x,y,z,value, **msg):
		pass
			
	### AGENT DELTA MESSAGES ###
	
	def agent_create(self, id, position, player_id, **msg):
		pass
		
	def agent_delete(self, id, **msg):
		pass
	
	def agent_position_change(self, id, position, **msg):
		pass

	def agent_state_change(self, id, position, version, **msg):
		pass
		
	### OBJECT DELTA MESSAGES ###
	
	def object_create(self, id, position, **msg):
		pass
		
	def object_delete(self, id, **msg):
		pass
	
	def object_position_change(self, id, position, version, **msg):
		pass

	def object_state_change(self, id, position, version, **msg):
		pass
