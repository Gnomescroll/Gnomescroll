#!/bin/sh
BROWSER="/Applications/Google\ Chrome.app/Contents/MacOS/Google\ Chrome"
PORT=8056

# Start redis server
redis-server redis.conf&

# Start html frontend server
python ./http_server/flask_server.py &

# Start node.js server
node ./node_server/server.js &

# Start game server
python ./game_server/server.py &

# Start html client
$BROWSER http://127.0.0.1:$PORT/
