#!/bin/sh
# Start redis server
BROWSER="chromium-browser"
PORT=8056
redis-server
# Start static HTML server
python ./html_server/static_server.py
# Start html frontend server
python ./html_server/server.py
# Start node.js server
node.js ./node_server/server.js
# Start game server
python ./game_server/server.py
# Start html client
# $BROWSER http://127.0.0.1:$PORT/
