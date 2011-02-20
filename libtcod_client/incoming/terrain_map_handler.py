
class Terrain_map_handler:

	def __init__(self):
		self.terrain_map = None
		pass

	def init_terrain_map(self, map, z_level, x_size, y_size, world_id, client_id, **msg):
		print "Terrain Map Assignment"
		#self.terrain_map.x_size = x_size
		#self.terrain_map.y_size = y_size
		#self.terrain_map.map = map
		pass

	def set_terrain_map(self, x,y,z,value, **msg):
		self.terrain_map.set(x,y,z,value)
		pass
		
	
