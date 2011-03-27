class World_map:

	def __init__(self):
		self.globals = None
		self.delta = None

		self.x_size = 200
		self.y_size = 200
		self.map = [-1 for col in range(self.x_size * self.y_size) ] #-1 is unknown or not defined
		self.version = 0 #versioning for objects
		
	def init(self):
		self.world_id = self.globals.world_id
		#grab state from persistant store
		pass
		
	def get(self, x,y,z):
		return self.map[y * self.x_size + x]

	def set(self, x,y,z,value):
		
		# fix crashes for bad input
		if None in [x,y,z,value]:
			return
			
		self.map[y * self.x_size + x] = value
		self.delta.set_map(x,y,z,value)
		self.version = self.version + 1

	#needs to return an array, may be sparse
	def get_z_level(self, z = 0):
		return self.map
