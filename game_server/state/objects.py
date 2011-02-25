

crops_dat = {
	generic_crop: 
	{
		name : "generic_crop",
		template : 
		{
			name : "generic_crop",
			type : ["crop"],
			version : 0
		},
		harvestable : 0,
		grows_from : "generic_seeds",
		mature_time : 900,
		matures_into : "generic_mature_crop"
	},

	generic_mature_crop:
	{
		name : "generic_mature_crop"
	}




}

def create_crop(id, x,y,z,template = None, world_id = 0):
	templates = {
		generic_crop : {
				name : "generic_crop",
				type : "crop",
				version : 0	
				},
		
		blood_berries : {
				name : "generic_crop",
				type : "crop",
				version : 0
		}
	
	
	}
	
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

		if object_type == None:
			a = create_object(id, x, y, z, player_id=player_id, world_id = self.globals.world_id)
		if object_type == 'item':
			pass
		if object_type == 'crop':
			pass
		if object_type == 'plant':
			pass
		self.object_list[id] = a
		self.delta.object_create(id, position, a['type'])
		
	def delete(self, id):
		pass
