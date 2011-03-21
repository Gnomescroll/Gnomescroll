
## Drawing properties for tiles

#	tile_name: param.tile_name,
#	tile_id :  param.tile_id,	

#	tilemap_id:  param.tilemap_id,
#	draw_style: param.draw_style,
#	background_rgb:  param.background_rgb,
#	symbol:  param.symbol,
#	symbol_rgb: param.symbol_rgb,


tiles_dat = [
			
	#empty things
	{
			'id' : -0001,
			'name' : 'unknown_tile',
			
			'visual' : {
				'tilemap' : {
					'tilemap_id' : 0,
					'draw_style' : 1,
					'symbol' : 00,
					'symbol_rgb' : (0, 0, 0),
					'background_rgb' : (256, 256, 256)
				}
			},
		
			'blocking' : 0,
			'pourous' : 1,
	
			'till' : 0,
			'dig' : 0
	},

	{
			'id' : 0001,
			'name' : "empty_block",

			'visual' : {
				'tilemap' : {
					'tilemap_id' : 0,
					'draw_style' : 1,
					'symbol' : 96,
					'symbol_rgb' : (50, 50, 50),
					'background_rgb' : (256, 256, 256)
				}
			},

			'blocking' : 0,
			'pourous' : 1,
			
			#actions
			'till' : 0,
			'dig' : 0
	},

	# soils
	{
			'id' : 0100,
			'name' : "generic_soil",

			'visual' : {
				'tilemap' : {
					'tilemap_id' : 0,
					'draw_style' : 1,
					'symbol' : 96,
					'symbol_rgb' : (139, 69, 19), #139-69-19
					'background_rgb' : (256, 256, 256)
				}
			},
						
			'blocking' : 0,
			'pourous' : 0,
			
			#actions
			'till' : 1,
			'till_into' : 'generic_tilled_soil',

			'dig' : 1,
			'dig_produces' : [],
			'dig_into' : 'empty_block'
	},

	{
			'id' : 0101,
			'name' : "generic_tilled_soil",

			'visual' : {
				'tilemap' : {
					'tilemap_id' : 0,
					'draw_style' : 1,
					'symbol' : 248,
					'symbol_rgb' : (139, 69, 19), #139-69-19
					'background_rgb' : (256, 256, 256)
				}
			},
			
			'blocking' : 0,
			'pourous' : 0,
			'vertical' : 1,
			
			#actions
			'till' : 0,
			
			'dig' : 0
			#'dig_produces' : [],
			#'dig_into' : 'empty_block'
	},

	# walls
	{
			'id' : 0200,
			'name' : "generic_wall",

			'visual' : {
				'tilemap' : {
					'tilemap_id' : 0,
					'draw_style' : 1,
					'symbol' : 9,
					'symbol_rgb' : (139, 69, 19), #139-69-19
					'background_rgb' : (256, 256, 256),
					
					'connection_map' : 1,
					'normal_connection_map' : 
					[ [0, 0, 0],
					 [0, 0, 0],
					 [0, 0, 0] ]
				}
			},
			
			#properties
			'blocking' : 1,
			'pourous' : 0,
			
			#actions
			'till' : 0,
			
			'dig' : 1,
			'dig_produces' : [],
			'dig_into' : 'generic_floor',
			
			'buildable' : 1,
			'build_requires' : []
	},

	{
			'id' : 0201,
			'name' : "clay_brick_wall",
			'blocking' : 0,
			'pourous' : 0,
			
			#actions
			'till' : 0,
			
			'dig' : 1,
			'dig_produces' : [],
			'dig_into' : 'generic_floor',
			
			'buildable' : 1,
			'build_requires' : ['clay_brick']
	},
	
	# floors
	{
			'id' : 0300,
			'name' : "generic_floor",
			'blocking' : 1, #can agents walk through it
			'pourous' : 1, #can fluids walk through it
			'gravity' : 0, #does it fall if not supporting something
			'vertical' : 0, #does it prevent the thing above it from falling
			
			#actions
			'till' : 0,
			
			'dig' : 0,
			#'dig_produces' : [],
			#'dig_into' : 'empty_block',
	},

	#blocks
	{
	
			'id' : 0400,
			'name'     : 'generic_stone_block',
			'blocking' : 1,
			'porous'   : 0,
			'vertical'  : 1,
			
			'till'   : 0,
			'dig': 1,
			'dig_produces' : [ (1,1, 'generic_stone') ],
			'dig_into' : 'generic_floor'
	},

	{
			'name'     : 'generic_soil_block',
			'blocking' : 1,
			'porous'   : 0,
			'vertical'  : 1,
			
			'till'   : 0,
			'dig': 0
			#'dig_produces' : [],
			#'dig_into' : 'generic_soil'
	},

	{
			'name' : "generic_wood_block",
			'blocking' : 1,
			'pourous' : 0,
			'vertical' : 1,
			
			#actions
			'till' : 0,
			'dig' : 0
	},
	
	{
			'name' : "generic_clay_block",
			'blocking' : 1,
			'pourous' : 0,
			'vertical' : 1,
			
			#actions
			'till' : 0,

			'dig': 1,
			'dig_produces' : [ (1,1, 'clay') ],
			'dig_into' : 'generic_floor'
	}		
]

crafting_dat = [

	{
		'name' : 'bake bread',
		'workshop' : 'oven',
		'reagent' : ['generic_food'],
		'product' : ['wheat_bread']
	}

]

workshop_dat = {
	
	'place_holder' : #used as a 1x1 square
	{
		'size' : (1,1),
		'template_params' : ['id','position', 'version', 'world_id', 'part_of', 'name'],
		'template' : 
		{
			'type' : ["building"]
		}				
	},
	
	'generic_workshop' : 
	{
		'name' : 'generic_workshop',
		'size' : (3,3),
		'active_square' : (1,1),
		'template_params' : ['id','position', 'version', 'world_id'],
		'template' : 
		{
			'name' : "generic_workshop",
			'workshop' : "generic_workshop",
			'type' : ["workshop", "building"]
		}			
	},
	
	'oven' : 
	{
		'name' : 'oven',
		'size' : (3,3),
		'active_square' : (1,1),
		'template_params' : ['id','position', 'version', 'world_id', 'active'],
		'template' : 
		{
			'name' : "oven",
			'workshop' : 'oven',
			'type' : ["workshop", "building"]
		}			
	}

}

crops_dat = {
	'generic_crop' : 
	{
		'template_params' : ['id','position', 'version', 'world_id'],
		'template' : 
		{
			'name' : "generic_crop",
			'type' : ["crop"],
			'parent' : "generic_crop",
		},

		'plantable' : 1,
		'planting_exp' : 20 ,
		'planting_difficulty' : 1,
		'grows_from' : "generic_seeds",
		
		'harvestable' : 0,
		'matures' : 1,
		'mature_time' : 40,
		'matures_into' : "generic_mature_crop"
	},

	'generic_mature_crop' :
	{
		'template_params' : ['id','position', 'version', 'world_id'],
		'template' : 
		{
			'name' : "generic_mature_crop",
			'type' : ["crop"],
			'parent' : "generic_mature_crop",
		},
		'plantable' : 0,
		'harvestable' : 1,
		'harvest_produces' : [(1,3, 'generic_food')], # 1d3 generic food
		'harvest_exp' : 10,
		'matures' : 0, #disable maturing for now
		'mature_time' : 5000,
		'matures_into' : "generic_dead_crop"
	},

	'generic_dead_crop' :
	{
		'template_params' : ['id','position', 'version', 'world_id' ],
		'template' : 
		{
			'name' : "generic_dead_crop",
			'type' : ["crop"],
			'parent' : "generic_dead_crop",
			#version : 0
		},
		'plantable' : 0,
		'harvestable' : 1,
		'harvest_produces' : [],
		'harvest_exp' : 5,
		'matures' : 0
	}
	
}

items_dat = {
	'generic_item' :
	{
		'template_params' : ['id','position', 'version', 'world_id' ],
		'template' : 
		{
			'name' : "generic_item",
			'type' : ["item"],
			'parent' : "generic_item",
			'material' : 'generic_material',
			'flammable' : 1
		},
		'weight' : 1,
		'value' : 1,
		'edible' : 0,
	},

	'generic_stone' :
	{
		'template_params' : ['id','position', 'version', 'world_id' ],
		'template' : 
		{
			'name' : "generic_stone",
			'type' : ["item"],
			'parent' : "generic_item",
			'material' : 'generic_stone',
			'flammable' : 0
		},
		'weight' : 50,
		'value' : 1,
		'edible' : 0,
	},
	
	'generic_food' :
	{
		'template_params' : ['id','position', 'version', 'world_id' ],
		'template' : 
		{
			'name' : "generic_food",
			'type' : ["item"],
			'parent' : "generic_food",
			'material' : 'generic_material',
			'flammable' : 1
		},
		'weight' : 1,
		'value' : 5,
		'edible' : 1,
		'food_value' : 15,
		'status_effects' : []
	},

	'wheat_bread' :
	{
		'template_params' : ['id','position', 'version', 'world_id' ],
		'template' : 
		{
			'name' : "wheat_bread",
			'type' : ["item"],
			#'parent' : "generic_food",
			'material' : 'generic_material',
			'flammable' : 1
		},
		'weight' : 1,
		'value' : 100,
		'edible' : 1,
		'food_value' : 100,
		'status_effects' : []
	},

## building materials

	'clay' :
	{
		'template_params' : ['id','position', 'version', 'world_id' ],
		'template' : 
		{
			'name' : "clay",
			'type' : ["item"],
			'material' : 'generic_material',
			'flammable' : 0
		},
		'weight' : 5,
		'value' : 3,
		'edible' : 0
	},

	'clay_brick' :
	{
		'template_params' : ['id','position', 'version', 'world_id' ],
		'template' : 
		{
			'name' : "clay_brick",
			'type' : ["item"],
			'material' : 'generic_material',
			'flammable' : 0
		},
		'weight' : 5,
		'value' : 25,
		'edible' : 0
	}	
}

class Dat:
	
	tiles_by_value = {}
	tiles_by_name = {}
	tile_name_value_pairs = []

	def __init__(self):
		self.crops_dat = crops_dat
		self.items_dat = items_dat
		self.workshop_dat = workshop_dat
		self.crafting_dat = crafting_dat
		
		self._init_tile()
	#crop helper

	def _init_tile(self):
		id = 1000
		for x in tiles_dat:
			if not 'id' in x.keys():
				print "Tile does not have id defined: " + x['name']
			
				x['id'] = id
				self.tiles_by_value[id] = x
				self.tiles_by_name[x['name']] = x
				self.tile_name_value_pairs.append((id, x['name']))
				id = id + 1
			else:
				id_ = x['id']
				self.tiles_by_value[id_] = x
				self.tiles_by_name[x['name']] = x
				self.tile_name_value_pairs.append((id_, x['name']))				
		#print "!!! START !!!"
		#print str(self.tiles_by_name)
		#print "!!! MIDDLE !!!"
		print "Tile name/value pairs: " + str(self.tile_name_value_pairs)
		#print "!!! END !!!"		
		
	def get_crop(self, template):
		if not template in self.crops_dat.keys():
			template = 'generic_crop'
		return self.crops_dat[template].copy()
				
	def get_crop_template(self, template):
		if not template in self.crops_dat.keys():
			template = 'generic_crop'
		return self.crops_dat[template]['template'].copy()

	#item helper
	def get_item(self, template):
		if not template in self.items_dat.keys():
			template = 'generic_item'
		return self.items_dat[template].copy()

	def get_item_template(self, template):
		if not template in self.items_dat.keys():
			template = 'generic_item'
		return self.items_dat[template]['template'].copy()

	#workshop helper
	def get_workshop(self, template):
		if not template in self.workshop_dat.keys():
			template = 'generic_workshop'
		return self.workshop_dat[template].copy()
	
	#crafting helper
	
	def get_crafting_recipe(self, recipe):
		for x in self.crafting_dat:
			if x['name'] == recipe:
				return x.copy()
		print "Crafting Recipe Does Not Exist: " + str(recipe)
		return None
		
		
		
	#Deprecated
#	def get_tile(self, value):
#		if value not in self.tiles_day.key():
#			print "Tile Does not Exist"
#			return self.tiles_dat[0]
#		return self.tiles_dat[value]
		
	def get_tile_by_name(self, name):
		if name not in self.tiles_by_name.keys():
			print "Tile Does not Exist: " + str(name)
			return self.tiles_by_name['unknown_tile']
		else:
			return self.tiles_by_name[name]
	
	def get_tile_by_value(self, value):
		if value not in self.tiles_by_value.keys():
			print "Tile Does not Exist: " + str(value)
			return self.tiles_by_name['unknown_tile']
		else:
			return self.tiles_by_value[value]

### Info

	def get_tiles(self):
		return self.tiles_by_value.copy()

### Rendering Information
	
	def get_tile_rendering(self):
		dict = {}
		default_tile_visual = {
			'tilemap' : {
				'tilemap_id' : 0,
				'draw_style' : 1,
				'symbol' : 63,
				'symbol_rgb' : (256, 0, 0),
				'background_rgb' : (0, 0, 0)
			},
		}
			
		for id, x in self.tiles_by_value.items():
			if 'visual' in x.keys():
				dict[id] = x['visual'].copy()
				dict[id]['tile_id'] = id
				dict[id]['tile_name'] = x['name']
			else:
				dict[id] = default_tile_visual
				dict[id]['tile_id'] = id
				dict[id]['tile_name'] = x['name']
		return dict
