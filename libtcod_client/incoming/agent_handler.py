
class Agent_handler:

	def __init__(self):
		self.Agents = None
		self.world_id = None
		self.client_id = None

	def agent_info(self, id, client_id, value, **msg):
		print "agent_info called."	

	def agent_position_change(self, id, position, **msg):
		for element in self.Agents:
			if element[id] = id:
				element[position] = position
				break

	def agent_state_change(self, id, position, version ,**msg):
		for element in self.Agents:
			if element[id] = id:
				element[version] = version
				break

	def agent_list(self, list, world_id, client_id, **msg):
		self.Agents = list
		self.world_id = world_id
		self.client_id = client_id

	def agent_create(self, id, position, player_id, **msg):
		self.Agents.append({"player_id": player_id, "position": position, "version": 0, "type": ["agent"], "id": id})
		
	def agent_delete(self, id, **msg):
		for element in self.Agents:
			if element[id] = id:
				del element
				break
