import time
class Terrain_map:

	def __init__(self):
		self.globals = None
		self.x_size = None
		self.y_size = None
		self.map = None #[-1 for col in range(self.x_size * self.y_size) ] #-1 is unknown or not defined
		self.version = 0 #versioning for objects
		
	def init(self):
		self.world_id = self.globals.world_id
		#grab state from persistant store
		pass

	def get(self, x,y,z):
		return self.map[y * self.x_size + x]

	def set(self, x,y,z,value):
		self.map[y * self.x_size + x] = value
		self.delta.set_map(x,y,z,value)
		#self.version = self.version + 1

	#needs to return an array, may be sparse
	def get_z_level(self, z = 0):
		return self.map

	#returns a section of the map, starting at x, y, z, with width width, and height height
	#this could probably be rewritten with list comprehensions if I understood them better
	def get_map_section(self, x, y, z, width, height):
		map_section = []
		map_row = []
		for h in range(y, height+1):
			for w in range(x, width+1):
				index = h * self.x_size + w
				print "asking for index", index
				map_row[w] = self.map[index]
			map_section[h] = map_row
		return map_section




