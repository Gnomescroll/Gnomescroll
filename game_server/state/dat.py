tiles_dat = [

	{
			'name' : 'unknown_tile',
			'till' : 0,
			'dig' : 0
	},

	{
			'name' : "empty_block",
			'blocking' : 0,
			'pourous' : 1,
			
			#actions
			'till' : 0,
			'dig' : 0
	},


	{
			'name' : "generic_soil",
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
			'name' : "generic_wall",
			'blocking' : 0,
			'pourous' : 0,
			
			#actions
			'till' : 0,
			
			'dig' : 1,
			'dig_produces' : [],
			'dig_into' : 'generic_floor'
	},


	{
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


	{
			'name'     : 'generic_stone_block',
			'blocks' : 1,
			'porous'   : 0,
			'vertical'  : 1,
			
			'till'   : 0,
			'dig': 1,
			'dig_produces' : [ (1,1, 'generic_stone') ],
			'dig_into' : 'generic_floor'
	},

	{
			'name'     : 'generic_soil_block',
			'blocks' : 1,
			'porous'   : 0,
			'vertical'  : 1,
			
			'till'   : 0,
			'dig': 0
			#'dig_produces' : [],
			#'dig_into' : 'generic_soil'
	},
	
	{
			'name' : "generic_tilled_soil",
			'blocking' : 0,
			'pourous' : 0,
			'vertical' : 1,
			
			#actions
			'till' : 0,
			
			'dig' : 0
			#'dig_produces' : [],
			#'dig_into' : 'empty_block'
	},


	{
			'name' : "generic_wood_block",
			'blocking' : 1,
			'pourous' : 0,
			'vertical' : 1,
			
			#actions
			'till' : 0,
			'dig' : 0
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
		id = 0
		for x in tiles_dat:
			x['id'] = id
			self.tiles_by_value[id] = x
			self.tiles_by_name[x['name']] = x
			self.tile_name_value_pairs.append((id, x['name']))
			id = id + 1
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
		if name not in self.tiles_by_name.key():
			print "Tile Does not Exist"
			return self.tiles_by_name['unknown_tile']
		else:
			return self.tiles_by_name[name]
	
	def get_tile_by_value(self, value):
		if value not in self.tiles_by_value.key():
			print "Tile Does not Exist"
			return self.tiles_by_name['unknown_tile']
		else:
			return self.tiles_by_value[value]
