crops_dat = {
	'generic_crop' : 
	{
		'template_params' : ['id','position', 'version', 'world_id'],
		'template' : 
		{
			'name' : "generic_crop",
			'type' : ["crop"],
			'parent' : "generic_crop",
			#version : 0
		},

		'plantable' : 1,
		'planting_experience' : 20 ,
		'planting_difficulty' : 1,
		'grows_from' : "generic_seeds",
		
		'harvestable' : 0,
		'matures' : 1,
		'mature_time' : 900,
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
			#version : 0
		},
		'plantable' : 0,
		'harvestable' : 1,
		'harvest_produces' : [(1,3, 'generic_food')], # 1d3 generic food
		'harvest_experience' : 10,
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
		'harvest_experience' : 5,
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
	}
	
}

class Dat:
	
	def __init__(self):
		self.crops_dat = crops_dat
		self.items_dat = items_dat

	#crop helper

	def get_crop(self, template):
		if not template in self.crops_dat.keys():
			template = 'generic_crop'
		return self.crops_dat[template]
				
	def get_crop_template(self, template):
		if not template in self.crops_dat.keys():
			template = 'generic_crop'
		return self.crops_dat[template]['template']

	#item helper
	def get_item(self, template):
		if not template in self.items_dat.keys():
			template = 'generic_item'
		return self.item_dat[template]

	def get_item_template(self, template):
		if not template in self.items_dat.keys():
			template = 'generic_item'
		return self.crops_date[template]['template']
