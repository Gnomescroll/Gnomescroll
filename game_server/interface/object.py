custom_object_attributes = [] #attributes with class defined get/set properties

class Nobject:

	world_map = None
	delta = None
	agents = None
	objects = None
	world_time = None
	dat = None

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
		if name in custom_object_attributes:
			property = self.__class__.__dict__[name] #Agent == self.__class__
			property.fset(self, value)
		else:
			self.__dict__['object_dict'][name] = value

	# does not over right the property methods, unlike setattr
	def __getattr__(self, name):
		#print "oops"
		if name in custom_object_attributes:
			print "object, __getattr__, Should never occur! Revise assumptions"
			property = self.__class__.__dict__[name]
			value = property.fget(self)
			return value
		else:
			return self.__dict__['object_dict'][name]

	def __init__(self, id):
		self.__dict__['object_dict'] = self.__class__.objects.object_list[id]
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
