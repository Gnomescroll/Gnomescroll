from database_interface import *



class Tile:
	
	object_type = "tile"
	
	#key to storage
	object_schema = {
	'meta' : "/meta",
	'admin' : "/admin",
	'visual_tilemap' : '/visual/tilemap',
	'properties' : '/properties',
	'actions' : '/actions',
	}
	
	def __init__(self, id = None):
		pass			
		
###
	def create_new_tile(self):
		object_id = get_free_id(self.object_type)
		assert type(object_id) == type(0)
		
		default_tile =	{
					'meta' : {
						'type' : 'tile',
						'id' : object_id,
						'name' : "default_tile",
					},
					
					'admin' : {
						'active' : 0,
					},
					
					'visual_tilemap' : {
						'tilemap_id' : 0,
						'draw_style' : 1,
						'symbol' : 63,
						'symbol_rgb' : (50, 0, 0),
						'background_rgb' : (0, 0, 0)
					},
					
					'properties' : {
						'blocking' : 0,
						'pourous' : 0,
						'vertical' : 1,
					},
					
					'actions' : {
					#actions
					'till' : 0,
					
					'dig' : 0,
					
					}
			}
			
		dict_to_redis(self.object_type, object_id, default_tile)
		add_object_to_index(self.object_type, object_id)
		return object_id
		
	def get(self, object_id):
		assert type(object_id) == type(0)
		dict = dict_from_redis(object_id, self.object_type)
		
	def get_all_keys(self):
		lista = get_object_list(self.object_type)
		return lista
		
	def get_all(self):
		key_list = get_object_list(self.object_type)
		result_list = []
		for key in key_list:
			result_list.append(dict_from_redis_by_key(key))
		return result_list
