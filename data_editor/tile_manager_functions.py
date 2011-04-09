from database_interface import 

from util import serialize

class Tile:
	
	type = "tile"
	id_counter = "tile_id"
	index = "tile_index"
	
	def __init__(self, id = None):
		self.r = redis.Redis('localhost')
		if id == None:
			self.id = int(r.incr(self.id_counter))
			print "tile_id: " + str(self.id)
			self.create_default(self.id)
		else:
			self.id = id
			self.load(id)
		
	def create_default(id):
		assert type(id) == type("0")

		d =	{
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
		self.tile = d
		
		output_list = []
		output_dictionary = {}
		serialize(d, id, this.type, output_list, output_dictionary):
		
		
		#self.r.set(self.prefix+str(self.id))
		
	def load(self, id):
		key = self.prefix + str(id)
		self.r.get(key)
		
		
	def save(self, id, value):
		key = self.prefix + str(id)
		data = simplejson.loads(value, encoding = 'utf-8')
		self.r.set(key, data)
