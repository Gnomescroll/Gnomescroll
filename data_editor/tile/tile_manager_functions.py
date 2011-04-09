from database_interface import *



class Tile:
	
	object_type = "tile"

	def __init__(self, id = None):
		pass

	def create_new_tile(self):
		id = get_free_id(self.object_type)
		assert type(id) == type(0)
		
		default_tile =	{
					'type' : 'tile',
					'id' : id,
					'name' : "default_tile",
					
					'admin' : {
						'active' : 0,
					},
					
					'visual' : {
						'tilemap' : {
							'tilemap_id' : 0,
							'draw_style' : 1,
							'symbol' : 63,
							'symbol_rgb' : (50, 0, 0),
							'background_rgb' : (0, 0, 0)
						}
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
					#'dig_produces' : [],
					#'dig_into' : 'empty_block'
			}
			
		dict_to_redis(self.object_type, id, default_tile)
		add_object_to_index(self.object_type, id)
		return id
		
	def get(self, id):
		object_type = self.object_type
		output_dict = {}
		meta_info_dict = {}
		dict_from_redis(id, object_type, output_dict, meta_info_dict)
		
	def get_all_keys(self):
		object_type = self.object_type
		lista = get_object_list(object_type)
		return lista
		
	def get_all(self, id):
		object_type = self.object_type
		lista = get_object_list(object_type)
		results = []
		for key in lista:
			t0 = {}
			t1 = {}
			dict_from_redis_by_key(key, object_type, t0, t1)
			results.append((t0,t1))
		return results
