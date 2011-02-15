#imports

class Info_commands:
	
	def __init__(self):
		pass

	#required = ['agent_id', 'client_id', 'world_id']
	#optional = []		
	def get_agent(self, agent_id, client_id, **msg):
		self.info.agent_info(agent_id, client_id)

	#required = ['object_id', 'client_id', 'world_id']
	#optional = []			
	def get_object(self, **msg):
		self.info.object_info(object_id, client_id)

	#required = ['client_id', 'world_id']
	#optional = ['z']
	def get_map(self, client_id, z=0, **msg):
		self.info.get_map(z, client_id)

	#required = ['client_id', 'world_id']
	#optional = []
	def get_agent_list(self, client_id, **msg):
		self.info.get_agent_list(client_id)

	def get_object_list(self, client_id, **msg):
		self.info.get_object_list(client_id)
