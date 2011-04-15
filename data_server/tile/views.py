from django.http import HttpResponse, HttpResponseRedirect
from django.template import Context, loader

from data_server.tile.models import *

import sys
import simplejson
import ast

def tile_list(request):
	tiles = Tile.objects.all()
	tile_list = tiles
	#tile_list = []
	t = loader.get_template('tile/tile_list.html')
	c = Context({
        'tile_list': tile_list,
    })
	return HttpResponse(t.render(c))
	
def edit_tile(request, tile_id):
	##Stuff
	tile = None
	t = loader.get_template('tile/edit_tile.html')
	c = Context({
        'tile_id': tile_id,
		'tile' : tile
    })
	return HttpResponse(t.render(c))

def edit_tile(request, tile_id):
	tile = Tile()
	tile.save()
	return HttpResponse("Success")

def API_get_tile(request, tile_id):
	tile = Tile.objects.get(id = tile_id)
	dict = tile.to_dict()
	return HttpResponse(str(dict))
	
def API_get_tile_visual(request, tile_id):
	tile = Tile.objects.get(id = tile_id)
	dict = ast.literal_eval(tile.visual_tilemap)
	return HttpResponse(str(dict))

def API_get_tile_id_list(request):
	tiles = Tile.objects.all()
	id_list = []
	for tile in tiles:
		id_list.append(tile.id)
	return HttpResponse(str(id_list))

def API_get_tile_list(request):
	tiles = Tile.objects.all()
	id_list = []
	for tile in tiles:
		id_list.append(tile.id)
	return HttpResponse(str(id_list))
