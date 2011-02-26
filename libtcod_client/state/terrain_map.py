import time
class Terrain_map:

	def __init__(self):
		self.globals = None
		self.x_size = None
		self.y_size = None
		self.map = None #[-1 for col in range(self.x_size * self.y_size) ] #-1 is unknown or not defined
		self.version = 0 #versioning for objects
		self.redraw = True
		
	def init(self):
		self.world_id = self.globals.world_id
		#grab state from persistant store
		pass

	def get(self, x,y,z):
		return self.map[y * self.x_size + x]

	def set(self, x,y,z,value):
		self.map[y * self.x_size + x] = value
		#self.delta.set_map(x,y,z,value)
		self.version = self.version + 1
		self.redraw = True

	#needs to return an array, may be sparse
	def get_z_level(self, z = 0):
		return self.map

	#returns a section of the map, starting at x, y, z, with width width, and height height
	def get_map_section(self, x, y, z, width, height):
		if self.map == None:
			print "map is not initialized!"
		
		map_section = []
		for w in range(x, width):
			map_row = []
			for h in range(y, height):
				index = h * self.x_size + w
				map_row.insert(w, self.map[index])
			map_section.insert(h, map_row)
		return map_section




