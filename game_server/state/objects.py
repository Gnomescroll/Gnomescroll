

#creates an oject from a template
def create_object(id, x, y, z, template = None, player_id = 0, world_id = 0):
	#if prototype == None:
	#	pass
	a = {}
	a['id'] = id
	a['type'] = ['item']
	a['name'] = 'generic item'
	#position is (type, x, y, z)
	a['position'] = [0 , x, y, z] #type zero is ground
	a['player_id'] = player_id
	a['world_id'] = world_id
	a['version'] = 0
	return a
	
class Objects:

	def __init__(self):
		self.globals = None
		self.delta = None
		#
		self.object_list = {}

	def init(self):
		world_id = self.globals.world_id
		#grab state from persistant store
		pass

	def create(self, position, object_type = None, template = None, player_id=0):
		(position_type, x, y, z) = position
		
		id = self.globals.get_unique_id()
		a = create_object(id, x, y, z, player_id=player_id, world_id = self.globals.world_id)
		self.object_list[id] = a
		self.delta.object_create(id, position, a['type'])
		
	def delete(self, id):
		pass
