required_agent_attributes = [ 'x','y','z','position_type', 'name', 'id', 'player_id', 'world_id' ]
optional_agent_attributes = [ 'previous_player_id' ]
custom_get_set_agent_attributes = ['position'] #attributes with class defined get/set properties
noset_agent_attributes = ['inventory'] #variables that cannot be set with setter method

native_agent_attributes = ['agents', 'agent_dict'] #everything stored in the class instances has to be defined here

class Agent:

	world_map = None #world map object
	delta = None #delta object
	agents = None	 #agent dictionary
	
	#custom get/set methods
	def _get_position(self):
		print "Works till here"
		print "gx = " + str(self.gx)
		return (self.gx, self.gy, self.gz)
	def _set_position(self, (gx,gy,gz)):
		self.gx = gx
		self.gy = gy
		self.gz = gz
	position = property(_get_position, _set_position)

	def __setattr__(self, name, value):
		if name in native_agent_attributes:
			self.__dict__[name] = value
		elif name in noset_agent_attributes:
			print "Error: Agent, __setattr__: tried to write a non-settable parameter"
			return False #should throw an attribute error
		elif name in self.__class__.__dict__ and name in custom_get_set_agent_attributes:       # check to see if there is a property object
			property = self.__class__.__dict__[name]
			property.fset(self, value)
			return True
		elif name in required_agent_attributes or name in optional_agent_attributes:
			self.agent_dict[name] = value
			return True
		else:
			print "Agent: __setattr__, attribute not defined in lists: " + str(name)
			return False
			
	# does not over right the property methods, unlike setattr
	def __getattr__(self, name):
		#print "oops"
		if name in custom_get_set_agent_attributes:
			property = self.__class__.__dict__[name]
			value = property.fget(self)
			return value
		elif name in native_agent_attributes:
			return self.__dict__[name]
		elif name in required_agent_attributes:
			return self.agent_dict[name]
		elif name in optional_agent_attributes:
			if name in self.agent_dict[name]:
				return self.agent_dict[name]
			else:
				return None #if key does not exist

	def __delattr__(self, name):
		if name in required_agent_attributes:
			print "Warning: Agent.__delattr__(self, name) : deleting required agent attribute, " + str(name)
			self.agent_dict[name] = None
			return True
		elif name in self.agent_dict.keys():
			del self.agent_dict[name]
			return True
		else:
			print "Warning: Agent.__delattr__(self, name) : trying to delete a non-existant agent attribute, " + str(name)
			return None

	def __init__(self, id):
		#print "Agent Init"
		self.agent_dict = Agent.agents[id]
		#print "Agent Init Finished" + str( self.agent_dict)
		#self.agents = None #this does nothing
