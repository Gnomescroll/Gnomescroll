
class Agent_handler:

	def __init__(self):
		self.Agents = None
		pass

	def agent_info(self, id, client_id, value, **msg):
		pass

	def agent_position_change(self, id, position, **msg):
		pass

	def agent_state_change(self, id, position, version ,**msg):
		pass

	def agent_list(self, list, world_id, client_id, **msg):
		pass

	def agent_create(self, id, position, player_id, **msg):
		pass
		
	def agent_delete(self, id, **msg):
		pass
