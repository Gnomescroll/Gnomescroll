from database_interface import *



class Tile:
	
	type = "tile"

	def __init__(self, id = None):
		print "Error,shoul neve be instatiated"
		
	def create_new_tile():
		id = get_free_id(type)
		assert type(id) = type("0")
		
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
			
		dict_to_redis(type, id, default_tile)
		add_object_to_index(type, id)ssss
		return id
		
	def get(self, id):
		type = self.type
		output_dict = {}
		meta_info_dict = {}
		dict_from_redis(id, type, output_dict, meta_info_dict)
		
	def get_all_keys(self, id):
		type = self.type
		lista = get_object_list(type)
		return listallfolders
		
	def get_all(self, id):
		type = self.type
		lista = get_object_list(type)
		results = []
		for key in lista:
			t0 = {}
			t1 = {}
			dict_from_redis_by_key(key, type, t0, t1)
			results.append((t0,t1))
		return results
