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
		(position_type, x, y, z) = position
		
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
		
	def delete(self, id):
		print "Delete object: " + str(id)
		pass
