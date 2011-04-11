from flask import Flask, render_template, request, Response
import simplejson
import redis
from marshal import dumps

#from database_interface import get_object_list, delete_object, dict_from_redis, dict_to_redis
#from database_interface import get_object_list, delete_object, dict_from_redis, dict_to_redis

from tile.tile_manager_functions import *

import string
# get_object_list(type)
# add_to_index(type, id)
# remove_from_index(type, id)
# delete_object(type, id)

# dict_from_redis(id, object_type, output_dict = {}, meta_info_dict = {})
# dict_to_redis(type, id, input_dict):

from tile.tile_manager import *

app = Flask(__name__)

PORT = 8060

@app.route("/")
def root():
	return "Use a URL"
	#return render_template('tile_list.html', tile_list = {})

@app.route("/tile/tile_list")
def tile_list():
	tile = Tile()
	return render_template('tile_list.html', tile_list = tile.get_all())

@app.route("/tile/tile_key_list")
def tile_key_list():
	print "asdfsfd"
	tile = Tile()
	tile_list = tile.get_all_keys()
	id_list = []
	for x in tile_list:
		(otype, oid) = string.split(x, ":")
		id_list.append(oid)
	return render_template('tile_list.html', tile_list = tile_list, id_list = id_list)
	
@app.route("/tile/create_tile")
def create_tile():
	tile = Tile()
	tile_id = tile.create_new_tile()
	return "Works: " + str(tile_id)
	#return redirect(url_for('edit_tile', tile_id = tile_id)

@app.route("/api/tile_edit")
def edit_field():
	(prefix, key) = string.split(ref, "//")
	(otype, oid) = string.split(prefix, ':')
	oid = int(oid)
	return "Works"

@app.route("/tile/edit/<int:tile_id>")
def edit_tile(tile_id):
	tile_raw = {}
	tile = Tile()
	#print str(type(tile.get(tile_id)))
	return render_template('tile_editor.html', 
		tile_dict = tile.get(tile_id),
		tile_raw = tile.get_raw(tile_id),
		tile_id = tile_id,	
		)
    

### 
  
@app.route('/api', methods=['POST'])
def api_call(msg=None):
	vars = request.form
	
	data = vars.get('json', {})
	data = simplejson.loads(data, encoding = 'utf-8')
	print str(data)

	send_message(data)
	return 'ok'
    
@app.route('/static/<subdir>/<file>')
def static_image(subdir='',file=None):
    print subdir
    print file
    ext = ''
    try:
        ext = file.split('.')[-1].lower()
    except KeyError:
        return Response('Unknown mimetype.')
    
    mimetype = 'text/plain'
    if ext in ['png', 'jpg', 'jpeg']:
        ext = 'jpeg' if ext == 'jpg' else ext
        mimetype = 'image/'+ext

    elif ext == 'js':
        mimetype = 'application/javascript'
    elif ext == 'css':
        mimetype = 'text/css'

    path = dir+'/static/'+subdir+'/'+file
    read_mode = 'r'
    if mimetype.split('/')[0] == 'image':
        read_mode += 'b'
    with open(path, read_mode) as f:
        return Response(f.read(), mimetype=mimetype)

        
        
if __name__ == '__main__':
    app.debug = True
    app.run(port=PORT)
