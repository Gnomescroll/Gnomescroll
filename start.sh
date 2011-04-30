#!/bin/sh
BROWSER="chromium-browser"
PORT=8080

# Start redis server
redis-server redis.conf&

# Start html frontend server
#python ./http_server/flask_server.py &

# Start game server
python ./game_server/server.py &

cd ./node_server/
node http.js &
node server.js &

# Start html client
$BROWSER http://127.0.0.1:$PORT/
