
custom_agent_attributes = ['position'] #attributes with class defined get/set properties

class Agent:

	agents = None	 #agent dictionary
	world_map = None #world map object
	delta = None #delta object
	
	#custom get/set methods
	def _get_position(self):
		return (self.x, self.y, self.z)
	def _set_position(self, (x,y,z)):
		self.x = x
		self.y = y
		self.z = z
	position = property(_get_position, _set_position)

	#override handling
	def __setattr__(self, name, value):
		if name in custom_agent_attributes:
			property = Agent.__dict__[name] #Agent == self.__class__
			property.fset(self, value)
		else:
			self.__dict__['agent_dict'][name] = value

	# does not over right the property methods, unlike setattr
	def __getattr__(self, name):
		#print "oops"
		if name in custom_agent_attributes:
			print "Agent, __getattr__, Should never occur! Revise assumptions"
			property = Agent.__dict__[name]
			value = property.fget(self)
			return value
		else:
			return self.__dict__['agent_dict'][name]

	def __init__(self, id):
		self.__dict__['agent_dict'] = Agent.agents[id]
		self.__dict__['id'] = id
	
	def serialize(self):
		return self.__dict__['agent_dict']

	#a simple serializer
	def simple_serialize(self):
		msg = {}
		msg['id'] = self.__dict__['id']
		msg['x'] = self.x
		msg['y'] = self.y
		msg['z'] = self.z
		msg['position_type'] = self.position_type
		msg['player_id'] = self.player_id
		msg['version'] = self.version
		return msg
