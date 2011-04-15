from django.http import HttpResponse, HttpResponseRedirect
from django.template import Context, loader

from data_server.tileset.models import *

import sys

def list_tileset(request):
	#stuff
	tileset_list = []
	t = loader.get_template('tileset/tileset_list.html')
	c = Context({
        'tileset_list': tileset_list,
    })
	return HttpResponse(t.render(c))
	
def edit_tileset(request, tileeset_id):
	##Stuff
	tileset = None
	t = loader.get_template('tileset/edit_tileset.html')
	c = Context({
        'tile_id': tileset_id,
		'tileset' : tileset
    })
	return HttpResponse(t.render(c))
