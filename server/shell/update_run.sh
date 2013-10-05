#!/bin/bash

# update repo
hg pull -u

# compile & copy
./waf configure --release=production --optimize=3 --auth
./waf
./shell/begin_server.sh
