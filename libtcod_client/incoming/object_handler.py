
class Object_handler:

	def __init__(self):
		self.objects = None
		pass

	def object_info(self, id, client_id, value, **msg):
		pass

	def object_position_change(self, id, position, version, **msg):
		pass
		
	def object_list(self, list, client_id, world_id, **msg):
		pass
	
	def object_create(self, id, position, **msg):
		pass
		
	def object_delete(self, id, **msg):
		pass
