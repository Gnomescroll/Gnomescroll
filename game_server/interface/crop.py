from interface.object import Nobject

custom_crop_attributes = [] #attributes with class defined get/set properties

class Crop:

	world_map = None
	delta = None
	agents = None
	objects = None
	world_time = None
	dat = None

	### Object Specific Functions ###
	
	def	mature(self):
		print "CROP MATURED!!!"
		pass
	
	def set_timer(self):
		print "Set Timer!!!"
		template = self.dat.get_crop(self.parent)
		if template['matures'] == 1:
			mature_time = int(template['mature_time'])
		
			timer = {}
			timer['id'] = self.__dict__['id']
			timer['timer_type'] = 'crop'
			timer['event_type'] = 'mature'
		
			self.world_time.add_timer(timer, mature_time)
		pass
		
	### GENERIC CRAP ###
	
	##custom get/set methods
	#def _get_position(self):
		#return (self.x, self.y, self.z)
	#def _set_position(self, (x,y,z)):
		#self.x = x
		#self.y = y
		#self.z = z
	#position = property(_get_position, _set_position)

	#override handling
	def __setattr__(self, name, value):
		if name in custom_crop_attributes:
			property = this.__class__.__dict__[name]
			property.fset(self, value)
		else:
			self.__dict__['object_dict'][name] = value

	# does not over right the property methods, unlike setattr
	def __getattr__(self, name):
		#print "oops"
		if name in custom_crop_attributes:
			print "crop, __getattr__, Should never occur! Revise assumptions"
			property = this.__class__.__dict__[name]
			value = property.fget(self)
			return value
		else:
			return self.__dict__['agent_dict'][name]

	def __init__(self, id):
		id = int(id)
		self.__dict__['object_dict'] = Nobject.objects.object_list[id]
		self.__dict__['id'] = id
	
	def serialize(self):
		return self.__dict__['object_dict']

	#a simple serializer
	def simple_serialize(self):
		msg = {}
		msg['id'] = self.__dict__['id']
		msg['position'] = self.position
		msg['type'] = self.type
		msg['version'] = self.version
		return msg
