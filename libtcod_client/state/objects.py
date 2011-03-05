class Objects:
	
	objects = None
	valid_locations = ['ground','agent','container']
	
	def __init__(self, object_id, object_type, location_state, **pls):
		self.type = object_type
		self.id = object_id
		self.location_state = location_state
		self.previous_owner = None
		if 'previous_location_state' in pls and isinstance(pls['previous_location_state'], dict):
			self.previous_location_state = pls['previous_location_state']
		else:
			self.previous_location_state = None
		
	def position(self):
		if 'ground' in self.location_state:
			gx = self.location_state['ground']['gx']
			gy = self.location_state['ground']['gy']
			gz = self.location_state['ground']['gz']
			return (gx,gy,gz)
		elif 'agent' in self.location_state:
			print 'Object is held by an agent.  Agent_id: ', self.location_state['agent']
		elif 'container' in self.location_state:
			print 'Object is in a container. Container_id: ', self.location_state['container']
		else:
			print 'Object has no position'
		return None
			
	def owner(self):
		if 'agent' in self.location:
			return self.location['agent']
		else:
			print 'Object has no owner'
			return None
			
	def container(self):
		if 'container' in self.location:
			return self.location['container']
		else:
			print 'Object is not in a container'
			return None
			
	def get_location_type(self):
		num = len(self.location_state)
		if num == 1:
			return self.location_state.keys()[0]
		elif num == 0:
			print 'Object has no location_state. Error.'
			return None
		elif num > 1:
			print 'Object has more than one location_type.  Error.'
			print 'Dumping location_state: ', self.location_state.items()
			return None
			
	def get_location_state(self):
		num = len(self.location_state)
		if num == 1:
			return self.location_state.items()[0]
		elif num == 0:
			print 'Object has no location_state.  Error.'
			return None
		elif num > 1:
			print 'Object has more than one location state.  Error.'
			print 'Dumping location_state: ', self.location_state.items()
			return None
			
	def change_location_state(self, location_state):
		if isinstance(location_state, dict):
			location_type = location_state.keys()[0]
			if location_type in self.valid_locations:
				self.previous_location_state = self.location_state
				self.location_state = location_state
				print 'Changed object location to: ', location_state
				if 'agent' in self.previous_location_state:
					self.previous_owner = self.previous_location_state['agent']
				if 'agent' in self.location_state and self.location_state['agent'] != self.previous_owner:
					print 'An item was stolen!!'
		else:
			print 'Invalid location state. Dumping: ', location_state
			
	def key(self):
		if self.get_location_type() == 'ground':
			gx = self.location_state['ground']['gx']
			gy = self.location_state['ground']['gy']
			px = gx % 256
			py = gy % 256
			cx = (gx - px) / 256
			cy = (gy - py) / 256
			key = (cx, cy)
			return key
		else:
			return None
	
	def map_n(self):
		if self.get_location_type() == 'ground':
			gx = self.location_state['ground']['gx']
			gy = self.location_state['ground']['gy']
			px = gx % 256
			py = gy % 256
			n = 256*py + px
			return n
		else:
			return None
			
	def gx(self):
		if self.get_location_type() == 'ground':
			return self.location_state['ground']['gx']
		else:
			return None
			
	def gy(self):
		if self.get_location_type() == 'ground':
			return self.location_state['ground']['gy']
		else:
			return None
			
	def gz(self):
		if self.get_location_type() == 'ground':
			return self.location_state['ground']['gz']
		else:
			return None
			
			
	def serialize(self):
		serialized = {}
		serialized['object_id'] = self.id
		serialized['location_state'] = self.location_state
		serialized['object_type'] = self.type
		if self.previous_owner != None:
			serialized['previous_owner'] = self.previous_owner
		else:
			serialized['previous_owner'] = 0
		if self.previous_location_state != None:
			serialized['previous_location_state'] = self.previous_location_state
		else:
			serialized['previous_location_state'] = 0
		return serialized

class Containers(Objects):
	
	objects = None
	valid_locations = ['ground','agent']
	
	def __init__(self, container_id, container_type, location_state, contents=[]):
		self.type = container_type
		self.id = container_id
		self.location_state = location_state
		self.previous_owner = None
		self.previous_location_state = None
		self.contents = contents
		
	def serialize(self):
		serialized = {}
		serialized['object_id'] = self.id
		serialized['location_state'] = self.location_state
		serialized['object_type'] = self.type
		if self.previous_owner != None:
			serialized['previous_owner'] = self.previous_owner
		else:
			serialized['previous_owner'] = 0
		if self.previous_location_state != None:
			serialized['previous_location_state'] = self.previous_location_state
		else:
			serialized['previous_location_state'] = 0
		serialized['contents'] = self.contents
		return serialized
