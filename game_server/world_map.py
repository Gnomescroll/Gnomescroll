class World_map:

	def __init__(self):
		self.world_id = 0
		self.x_size = 128
		self.y_size = 128
		self.map = [-1 for col in range(self.x_size * self.y_size) ] #-1 is unknown or not defined

	def get(x,y,z):
		return self.map[y * self.x_size + x]

	def set(x,y,z,value):
		self.map[y * self.x_size + x] = value
		self.delta.set_map(x,y,z,value)
		#creates notification
