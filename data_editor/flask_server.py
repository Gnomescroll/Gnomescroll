from flask import Flask, render_template, request, Response
import simplejson
import redis
from marshal import dumps

from database_interface import get_object_list, delete_object, dict_from_redis, dict_to_redis



# get_object_list(type)
# add_to_index(type, id)
# remove_from_index(type, id)
# delete_object(type, id)

# dict_from_redis(id, object_type, output_dict = {}, meta_info_dict = {})
# dict_to_redis(type, id, input_dict):


app = Flask(__name__)

PORT = 8060

@app.route("/")
def root():
    return render_template('tile_editor.html', {})

@app.rout("/tile_editor/tile_list")
def tile_list():
    return render_template('tile_editor.html', {})
	sss
@app.route("/tile_editor/create_tile")
def create_tile():
	return redirect(url_for('edit_tile', tile_id = Tile().id)
    #return render_template('tile_editor.html', {})

@app.route("/tile_editor/edit_tile/<tile_id>")
def edit_tile(tile_id):
    return render_template('tile_editor.html', {})
    

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
