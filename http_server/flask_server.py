from flask import Flask, render_template, request
import simplejson
import redis
from marshal import dumps

app = Flask(__name__)

dir = "html_client"
PORT = 8055

# redis
def send_message(msg):
    r_client = redis.Redis('localhost')
    world_id = msg['world_id']
    r_client.lpush("world_"+str(world_id), dumps(msg))

@app.route("/")
def root():
    
    return render_template(dir+'/index.html')
    
@app.route('/<file>')
def static_file(file=None):
    print file
    if file == 'favicon.ico': return ''
    return render_template(dir+'/'+file)
    
@app.route('/js/<file>')
def js_file(file=None):
    print file
    return render_template(dir+'/js/'+file, mimetype='application/javascript')
    
@app.route('/api/<msg>', methods=['POST'])
def api_call(msg=None):
    vars = request.form
    
    data = vars.get('json', {})
    print data
    data = simplejson.loads(data)

    send_message(data)

    return 'ok'
    
if __name__ == '__main__':
    app.debug = True
    app.run(port=PORT)
