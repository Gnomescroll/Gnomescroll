from django.http import HttpResponse, HttpResponseRedirect
from django.template import Context, loader

from data_server.tile.models import *

import sys

def list_tile(request):
	#stuff
	tile_list = []
	t = loader.get_template('tile/list.html')
	c = Context({
        'tile_list': tile_list,
    })
	return HttpResponse(t.render(c))
	
def edit_tile(request, tile_id):
	##Stuff
	tile = None
	t = loader.get_template('tile/edit.html')
	c = Context({
        'tile_id': tile_id,
		'tile' : tile
    })
	return HttpResponse(t.render(c))
