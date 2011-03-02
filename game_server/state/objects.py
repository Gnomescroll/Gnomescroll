#interfaces 
from interface.agent import Agent
from interface.object import Nobject
from interface.crop import Crop

def create_crop(id, x,y,z,template = None, world_id = 0):
	if template == None:
		template = 'generic_crop'
	a = crops_dat[template]
	
	#a = {}
	a['id'] = id
	#position is (type, x, y, z)
	a['position'] = [0 , x, y, z] #type zero is ground
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
		self.world_time = None
		self.dat = None
		
		self.object_list = {}

	def init(self):
		world_id = self.globals.world_id
		#grab state from persistant store
		pass

	def create(self, position, object_type = None, template = None, player_id=0):
		[position_type, x, y, z] = position
		
		id = self.globals.get_unique_id()

		if object_type == None:
			a = create_object(id, x, y, z, player_id=player_id, world_id = self.globals.world_id)
			self.object_list[id] = a
			self.delta.object_create(id, position, a['type'])
		if object_type == 'item':
			a = self._create_item(id, position, template)
		if object_type == 'crop':
			a = self._create_crop(id, position, template)
			#self.object_list[id] = a
			pass
		if object_type == 'plant':
			pass
		if object_type == 'workshop':
			a = self._create_workshop(id, position, template)
	
	def _create_crop(self, id, position, template):
		if template == None:
			template = 'generic_crop'
		a = self.dat.get_crop_template(template)
		a['id'] = id
		#position is (type, x, y, z)
		a['position'] = position
		a['world_id'] = self.globals.world_id
		a['version'] = 0
		self.object_list[id] = a
		self.delta.object_create(id, position, a['type'])
		Crop(id).set_timer()
		#tdict = self.dat.get_item(template)
		#if tdict['matures'] == 1:
		#	timer = 
		return a

	def _create_item(self, id, position, template):
		if template == None:
			template = 'generic_item'
		a = self.dat.get_item_template(template)
		a['id'] = id
		#position is (type, x, y, z)
		a['position'] = position
		a['world_id'] = self.globals.world_id
		a['version'] = 0
		self.object_list[id] = a
		self.delta.object_create(id, position, a['type'])
		#tdict = self.dat.get_item(template)
		#handle timers
		
		return a
	
	def _create_workshop(self, id, position, template):
		wt = self.dat.get_workshop(template)
		place_holder = self.dat.get_workshop(template)['template']
		
		active_square = wt['active_square']
		template = wt['template']
		(x_size, y_size) = wt['size']
		
		list = []
		for x in range(0,x_size):
			for y in range (0, y_size):
				list.append((x,y))
		
		
		## For each space, check to see if it can be built on and fail otherwise
		for (x,y) in list:
			p = (0, position[1] + x, position[2] +y, position[3])
			
			#check to see if square is blocked
			
		for (x,y) in  list:
			if (x,y) == active_square:
				temp = wt['template']
				temp['id'] = id
				id_t = id
				temp['position'] = (0, position[1] + x, position[2] +y, position[3]) #only 2d for now 
				temp['name'] = wt['name']
				temp['version'] = 0				
			else:
				temp = place_holder #make copy by value!!! else will cause errors
				id_t = self.globals.get_unique_id()
				temp['id'] = id_t
				temp['position'] = (0, position[1] + x, position[2] +y, position[3]) #only 2d for now 
				temp['name'] = "placeholder " + wt['name']
				temp['part_of'] = id
				temp['version'] = 0
			self.object_list[id] = temp
			self.delta.object_create(id_t, temp['position'], temp['type'])
			
	def delete(self, id):
		print "Delete object: " + str(id)
		del self.object_list[id]
		self.delta.object_delete(id)
