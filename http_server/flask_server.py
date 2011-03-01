from flask import Flask, render_template, request, Response
import simplejson
import redis
from marshal import dumps

app = Flask(__name__)

dir = "./html_client"
PORT = 8055

#r_client = redis.Redis('localhost')

# redis
def send_message(msg):
    r_client = redis.Redis('localhost')
    world_id = msg['world_id']
    r_client.lpush("world_"+str(world_id), dumps(msg))

@app.route("/")
def root():
    with open(dir+'/index.html') as f:
        return Response(f.read())
    
@app.route('/<file>')
def static_file(file=None):
    if file == 'favicon.ico': return ''
    with open(dir+'/'+file) as f:
        return Response(f.read())
    
@app.route('/js/<file>')
def js_file(file=None):
    with open(dir+'/js/'+file) as f:
        return Response(f.read(), mimetype='application/javascript')

@app.route('/api', methods=['POST'])
def api_call(msg=None):
    vars = request.form
    
    data = vars.get('json', {})
    data = simplejson.loads(data, encoding = 'utf-8')
    print str(data)

    send_message(data)

    return 'ok'
    
if __name__ == '__main__':
    app.debug = True
    app.run(port=PORT)
