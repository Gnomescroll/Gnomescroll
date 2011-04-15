from django.db import models

#id
#name
#visual_tile_map
#properties
#actions
##visual_isometric
##visual_3d

class Tile(models.Model):
	name = models.CharField(max_length=128)
	misc = models.TextField(default= '{}')
	visual_tilemap = models.TextField(default= '{}')
	visual_isometric = models.TextField(default= '{}')
	visual_3d = models.TextField(default= '{}')
	properties = models.TextField(default= '{}')
	actions = models.TextField(default= '{}')
	
	#meta-data crap
	created_at = models.DateTimeField(auto_now_add=True)
	deleted = models.BooleanField(default = False)
