class World_map:

	def __init__(self):
		self.world_id = 0
		self.x_size = 128
		self.y_size = 128
		self.map = [0 for col in range(self.x_size * self.y_size) ]

