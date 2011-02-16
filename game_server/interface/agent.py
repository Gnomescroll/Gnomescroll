import sys

#custom_agent_attributes = ['position'] #attributes with class defined get/set properties
custom_agent_attributes = []

class Agent:

	def move_0(self,dx,dy,dz):
		[type, x, y, z] = self.position
		[type, x, y, z] = [type, x+dx, y+dy, 0]
		self.position = [type, x, y, z]
		self.delta.agent_position_change(self.__dict__['id'], x, y, z)

	### Generic Stuff ###
	agents = None #agent dictionary
	objects = None	 #object dictionary
	world_map = None #world map object
	delta = None #delta object
	
	##custom get/set methods
	#def _get_position(self):
		#return self.position
	#def _set_position(self, position):
		#self.position = position
	#position = property(_get_position, _set_position)

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
		#print "id = " + str(id)
		try:
			id = int(id)
			self.__dict__['agent_dict'] = Agent.agents.agent_list[id]
			self.__dict__['id'] = id
		except Exception, err:
			print "Agent.__init__ , agent id does not exist %s: %s" %(sys.stderr, err)
			
	def serialize(self):
		return self.__dict__['agent_dict']

	#a simple serializer
	def simple_serialize(self):
		msg = {}
		msg['id'] = self.__dict__['id']
		msg['type'] = self.type
		msg['position'] = self.position
		msg['player_id'] = self.player_id
		msg['version'] = self.version
		return msg
