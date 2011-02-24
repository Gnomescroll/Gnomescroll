import sys

from interface.object import Nobject

#custom_agent_attributes = ['position'] #attributes with class defined get/set properties
custom_agent_attributes = []

class Agent:

	def move_0(self,dx,dy,dz):
		[type, x, y, z] = self.position
		[type, x, y, z] = [type, x+dx, y+dy, 0]
		self.position = [type, x, y, z]
		self.delta.agent_position_change(self.__dict__['id'], [0,x, y, z])

	def pickup_item(self, item_id):
		if self.holding == 1:
			print "Agent cannot hold multiple items!"
		else:
			item = Nobject(item_id)
			if self.position != item.position:
				print "Agent is too far from item!"
			else:
				item.position = (1, self.__dict__['id'])
				self.holding = item.id
				meta = { type : "pickup_item", item_id: item.id, agent_id: self.__dict__['id'] }
				self.delta.agent_state_change(self.__dict__['id'], self.position, self.verion, meta = meta )
				meta = {type : 'drop_item'}
				self.object_position_change(self.holding, self.position, meta=meta)
		pass

	def drop_item(self):
		if self.holding == 0:
			print "Agent is not holding anything!"
		else:
			item = Nobject(self.holding)
			item.position = self.position
			meta = {type : "drop_item", agent_id : self.__dict__['id'], object_id : self.holding}
			self.delta.agent_state_change(self.__dict__['id'], self.position, self.verion, meta = meta)
			meta = {type : 'drop_item'}
			self.object_position_change(self.holding, self.position, meta=meta)
			self.holding = 0
		pass

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
