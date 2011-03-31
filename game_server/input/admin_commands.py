#imports

class Admin_commands:
	
	def __init__(self):
		self.world_map = None
		self.agents = None
		self.objects = None
		self.globals = None
	
	# required = ['position']
	# optional = ['player_id']
	def create_agent(self, position, player_id = 0, **msg):
		(position_type, x, y, z) = position
		self.agents.create(x, y, z, player_id)
		pass
	
	# required = ['position', 'item_template']
	# optional = ['player_id']
	def create_item(self, position, item_template= None, **msg):
		position = list(position)
		type = "item"
		self.objects.create(position, object_type=type)
		pass

	# required = ['position,', 'object_type', 'template']
	# optional = ['player_id']
	def create_object(self, position, object_type, template = None, player_id = 0, **msg):
		position = list(position)
		self.objects.create(position, object_type, template , player_id)
		pass
		
	#required ['position', 'value', 'world_id']
	#optional []
	def set_map(self, position, value, **msg):
		(type, x, y, z) = position
		(type, x, y, z) = (int(type), int(x), int(y), int(z)) ##assert that inputs are integers
		value = int(value) #must be integers
		self.world_map.set(x, y, z, value)
