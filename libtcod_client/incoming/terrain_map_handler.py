
class Terrain_map_handler:

	def __init__(self):
		self.terrain_map = None
		pass

	def terrain_map(self, map, z_level, x_size, y_size, world_id, client_id, **msg):
		map = self.terrain_map 
		map.x_size = x_size
		map.y_size = y_size
		map.map = map
		pass

	def set_terrain_map(self, x,y,z,value, **msg):
		self.terrain_map.set(x,y,z,value)
		pass
		
	
