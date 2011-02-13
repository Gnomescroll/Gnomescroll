class World_map:

	def __init__(self):
		self.x_size = 128
		self.y_size = 128
		self.map = [-1 for col in range(self.x_size * self.y_size) ] #-1 is unknown or not defined
		self.version = 0 #versioning for objects
		
	def init(self, world_id =0):
		world_id = self.world_id
		#grab state from persistant store
		pass
		
	def get(x,y,z):
		return self.map[y * self.x_size + x]

	def set(x,y,z,value):
		self.map[y * self.x_size + x] = value
		self.delta.set_map(x,y,z,value)
		self.version = self.version + 1

	#needs to return an array, may be sparse
	def get_z_level(self, z = 0)
		return self.map
