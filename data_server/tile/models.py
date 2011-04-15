from django.db import models

import ast
import simplejson
import pprint

#id, name
#properties
#actions
#tilemap
#isometric
#axonometric

default_tile = 	{
			'id' : 0,
			'name' : 'default_tile',
			
			'visual' : {
				'tilemap' : {
					'tilemap_id' : 0,
					'draw_style' : 1,
					'symbol' : 63,
					'symbol_rgb' : (50, 0, 0),
					'background_rgb' : (0, 0, 0)
				},
				'isometric' : {},
				'axonometric' : {},
			},
		
			'actions' : {
				'till' : 0,
				'dig' : 0,			
			},
			
			'properties' : {
				'blocking' : 0,
				'pourous' : 1,			
			},

	},

#pp = pprint.PrettyPrinter(indent=4)

class Tile(models.Model):
	name = models.CharField(max_length=128, default= 'new_tile')
	misc = models.TextField(default= str())
	tilemap = models.TextField(default= pprint..default_tile['visual']['tilemap'])
	isometric = models.TextField(default= '{}')
	axonometric = models.TextField(default= '{}')
	properties = models.TextField(default= '{}')
	actions = models.TextField(default= '{}')
	
	#meta-data crap
	created_at = models.DateTimeField(auto_now_add=True)
	deleted = models.BooleanField(default = False)
	
	def to_dict(self):
		dict = {}
		dict['name'] = self.name
		dict['id'] = self.id
		dict['visual'] = {}
		dict['visual']['tilemap'] = ast.literal_eval(self.tilemap)
		dict['visual']['isometric'] = ast.literal_eval(self.isometric)
		dict['visual']['axonometric'] = ast.literal_eval(self.axonometric)
		dict['properties'] = ast.literal_eval(self.properties)
		dict['actions'] = ast.literal_eval(self.actions)
		return dict
		
	def to_json(self):
		return simplejson.dumps(self.to_dict())
