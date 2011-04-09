from database_interface import *



class Tile:
	
	object_type = "tile"

	def __init__(self, id = None):
		pass

	def create_new_tile(self):
		object_id = get_free_id(self.object_type)
		assert type(object_id) == type(0)
		
		default_tile =	{
					'type' : 'tile',
					'id' : object_id,
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
			
		dict_to_redis(self.object_type, object_id, default_tile)
		add_object_to_index(self.object_type, object_id)
		return object_id
		
	def get(self, object_id):
		assert type(object_id) == type(0)
		object_type = self.object_type
		assert type(object_type) == type("d ")
		output_dict = {}
		meta_info_dict = {}
		dict_from_redis(object_id, object_type, output_dict, meta_info_dict)
		
	def get_all_keys(self):
		object_type = self.object_type
		assert type(object_type) == type("d ")
		lista = get_object_list(object_type)
		return lista
		
	def get_all(self, result0, result1):
		print "start"
		object_type = self.object_type
		lista = get_object_list(object_type)
		print "lista: " + str(lista)
		for key in lista:
			#print "1: " + str(key)
			t0 = {}
			t1 = {}
			dict_from_redis_by_key(key, object_type, t0, t1)
			#print "2: " + str(t0)
			#print "3: " + str(t1)
			result0.append(t0)
			result1.append(t1)
		return None
