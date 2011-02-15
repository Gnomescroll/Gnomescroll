#imports

class Info_commands:
	
	def __init__(self):
		pass
		
	def get_agent(self, msg):
		pass
		
	def get_object(self, msg):
		pass

	#required = ['client_id', 'world_id']
	#optional = ['z']
	def get_map(self, z=0, msg**):
		self.info.get_map(z, client_id, world_id):
		pass

	#required = ['client_id', 'world_id']
	#optional = []
	def get_agent_list(self, client_id, msg**):
		self.info.get_agent_list(client_id)

	def get_object_list(self, client_id, msg**):
		self.info.get_object_list(client_id)
